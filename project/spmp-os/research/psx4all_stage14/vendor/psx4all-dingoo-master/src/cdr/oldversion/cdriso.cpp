#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#include <zlib.h>

#include "cdriso.h"
#ifdef SPMP_PSX4ALL
#include "../../spmp_port/spmp_psx_opt.h"
#ifndef DEBUGF
#define DEBUGF(...) ((void)0)
#endif
#endif

extern s8* packfile;

s8* IsoFile;
FILE *cdHandle = NULL;
u8* cdbuffer = NULL;
u8* cdlastbuffer;
u32 cdlastsector;
s32 fmode;
u8* Ztable = NULL;
int iscso = 0;
#ifdef SPMP_PSX4ALL
/* Actual sectors currently in the cache, rather than assuming full window. */
static unsigned spmp_cdr_cached_sectors = 0;
#endif

/* cso struct */
typedef struct _cso_struct
{
  unsigned char in_buff[2*2048];
  unsigned char out_buff[2048];
  struct {
    char          magic[4];
    unsigned int  unused;
    unsigned int  total_bytes;
    unsigned int  total_bytes_high; // ignored here
    unsigned int  block_size;  // 10h
    unsigned char ver;
    unsigned char align;
    unsigned char reserved[2];
  } header;
  unsigned int  fpos_in;  // input file read pointer
  unsigned int  fpos_out; // pos in virtual decompressed file
  int block_in_buff;      // block which we have read in in_buff
  int pad;
  int index[0];
}
cso_struct;
cso_struct *cso = NULL;

static int uncompress2(void *dest, int destLen, void *source, int sourceLen)
{
    z_stream stream;
    int err;

    stream.next_in = (Bytef*)source;
    stream.avail_in = (uInt)sourceLen;
    stream.next_out = (Bytef*)dest;
    stream.avail_out = (uInt)destLen;

    stream.zalloc = NULL;
    stream.zfree = NULL;

    err = inflateInit2(&stream, -15);
    if (err != Z_OK) return err;

    err = inflate(&stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        inflateEnd(&stream);
        return err;
    }
    //*destLen = stream.total_out;

    return inflateEnd(&stream);
}

s32 CDR_init() {
	return 0;
}

s32 CDR_shutdown() {
#ifdef SPMP_PSX4ALL
    return CDR_close(); /* Unloading the PSX core must release its CD cache. */
#else
    return 0;
#endif
}

s32 UpdateZmode() 
{
	s32 len = strlen(IsoFile);

	if (len >= 2) 
	{
		if (!strncasecmp(IsoFile+(len-2), ".z", 2)) 
		{
			return 1;
		}
	}

	if (len >= 4) 
	{
		if (!strncasecmp(IsoFile+(len-4), ".znx", 4)) 
		{
			return 3;
		}
	}
	
	return 0;
}

s32 CDR_open() {
    long filesize = 0;
    s32 Zmode;
#ifdef SPMP_PSX4ALL
    if (!packfile || !*packfile) return -1;
#endif

    if (cdHandle != NULL)
        return 0;  /* Do not allocate/leak another buffer for repeated opens. */
    if (cdbuffer) { free(cdbuffer); cdbuffer = NULL; }
    cdbuffer = (u8*)malloc(BUFFER_SIZE);
    if (!cdbuffer) return -1;
    memset(cdbuffer, 0, BUFFER_SIZE);
    cdlastbuffer = cdbuffer;
    cdlastsector = 0x7FFFFFFF;
#ifdef SPMP_PSX4ALL
    spmp_cdr_cached_sectors = 0;
#endif				/* it's already open */

	//cdbuffer = (u8*)malloc(BUFFER_SIZE);
	//if( cdbuffer == NULL)
	//	return -1;

	IsoFile = packfile;

	Zmode = UpdateZmode();

	if (Zmode) {
		FILE *f;
		s8 table[512];

		fmode = Zmode;
#ifdef PANDORA
		s8 shortfile[512];
		int shortlen;
		sprintf(shortfile, "%s", IsoFile);
		shortlen = strlen(shortfile);
	
		if(Zmode == 1)
		{
			if(shortlen > 3)
			{
				shortfile[shortlen-2] = '\0';
				if(shortlen < 8+2)
				{
					sprintf(table, "%sz~1.tab", shortfile);
				}
				else
				{
					sprintf(table, "%s.tab", shortfile);				
				}
			}
		}
		else if(Zmode == 3)
		{
			if(shortlen > 4)
			{
				shortfile[shortlen-4] = '\0';
				if(shortlen < 8+4)
				{
					sprintf(table, "%sznx~1.tab", shortfile);
				}
				else
				{
					sprintf(table, "%s.tab", shortfile);				
				}
			}
		}
		else if(Zmode == 2)
		{
			if(shortlen > 3)
			{
				shortfile[shortlen-3] = '\0';
				if(shortlen < 8+3)
				{
					sprintf(table, "%sbz~1.ind", shortfile);
				}
				else
				{
					sprintf(table, "%s.ind", shortfile);				
				}
			}
		}
		printf("Loading %s\n", table);
		f = fopen(table, "rb");
		if( f == NULL )
#endif
		{
			if (Zmode == 1 || Zmode == 3) sprintf(table, "%s.table", IsoFile);
			else sprintf(table, "%s.index", IsoFile);			
			f = fopen(table, "rb");
			if( f == NULL )
			{
					cdHandle = NULL;
					return -1;
			}
		}
		
		fseek(f, 0, SEEK_END);
		filesize = ftell(f);
		fseek(f, 0, SEEK_SET);
		Ztable = (u8*)malloc(filesize);	
		if (Ztable == NULL) 
		{
			SysPrintf("Error loading %s", table);
			gp2x_timer_delay(3000);
			SysPrintf("Couldn't allocate %ld bytes", filesize);
			gp2x_timer_delay(3000);
			cdHandle = NULL;
			return -1;
		}

		fread(Ztable, 1, filesize, f);
		fclose(f);
	} else {
		fmode = 0;
	}
	
	cdHandle = fopen(IsoFile, "rb");
        if (strlen(IsoFile)>=4 &&
            (!strncasecmp(IsoFile+(strlen(IsoFile)-4), ".cbn", 4) ||
             !strncasecmp(IsoFile+(strlen(IsoFile)-4), ".cso", 4)))
        {
                iscso = 1;
                cso_struct *tmp = NULL;
                int size;
                if (cdHandle == NULL)
                  goto cso_failed;

                cso = (cso_struct*)malloc(sizeof(*cso));
                if (cso == NULL)
                  goto cso_failed;

                if (fread(&cso->header, 1, sizeof(cso->header), cdHandle) != sizeof(cso->header))
                  goto cso_failed;

                if (strncmp(cso->header.magic, "CISO", 4) != 0) {
                  DEBUGF("cso: bad header");
                  goto cso_failed;
                }

                if (cso->header.block_size != CD_FRAMESIZE_RAW) {
                  DEBUGF("cso: block size (%u) != %d", cso->header.block_size, CD_FRAMESIZE_RAW);
                  goto cso_failed;
                }

                size = ((cso->header.total_bytes / CD_FRAMESIZE_RAW) + 1)*4 + sizeof(*cso);
                tmp = (cso_struct*)realloc(cso, size);
                if (tmp == NULL)
                  goto cso_failed;
                cso = tmp;
                DEBUGF("allocated %i bytes for CSO struct", size);

                size -= sizeof(*cso); // index size
                if (fread(cso->index, 1, size, cdHandle) != size) {
                  DEBUGF("cso: premature EOF");
                  goto cso_failed;
                }

                // all ok
                cso->fpos_in = ftell(cdHandle);
                cso->fpos_out = 0;
                cso->block_in_buff = -1;
        }
	if (cdHandle == NULL) {
		SysPrintf("Error loading %s\n", IsoFile);
		gp2x_timer_delay(1000);
		return -1;
	}

	return 0;

cso_failed:
#ifdef SPMP_PSX4ALL
        CDR_close(); /* Also free buffers, not just file descriptor. */
#else
        if (cso != NULL) free(cso);
        iscso = 0;
        if (cdHandle != NULL) fclose(cdHandle);
#endif
        return -1;
}

s32 CDR_close() {
    if (cdHandle) { fclose(cdHandle); cdHandle = NULL; }
#ifdef SPMP_PSX4ALL
    spmp_cdr_cached_sectors = 0;
#endif
	if(cdbuffer) { free(cdbuffer); cdbuffer = NULL; }

	if(Ztable) { free(Ztable); Ztable = NULL; }

	if (cso) { free(cso); cso = NULL; iscso = 0; }
	
	return 0;
}

// return Starting and Ending Track
// buffer:
//  byte 0 - start track
//  byte 1 - end track
s32 CDR_getTN(u8 *buffer) {
	buffer[0] = 1;
	buffer[1] = 1;

	return 0;
}

// return Track Time
// buffer:
//  byte 0 - frame
//  byte 1 - second
//  byte 2 - minute
s32 CDR_getTD(u8 track, u8 *buffer) {
	buffer[2] = 0;
	buffer[1] = 2;
	buffer[0] = 0;

	return 0;
}

// read track
// time : int 0 - minute ; int 1 - second ; int 2 - frame
// uses int format
s32 CDR_readTrack(u8 *time) {
	// check cache for current sector
	u32 curr_sector = MSF2SECT((time[0]), (time[1]), (time[2]));
	s32 i;

#ifdef SPMP_PSX4ALL
    if (!cdHandle || !cdbuffer) return -1;
    if (spmp_psx_cd_cache_hit(curr_sector, cdlastsector,
                              spmp_cdr_cached_sectors))
#else
    if (curr_sector >= cdlastsector && curr_sector < cdlastsector + BUFFER_SECTORS)
#endif
	{
#ifdef SPMP_PSX4ALL
        cdlastbuffer = cdbuffer + (curr_sector - cdlastsector) * CD_FRAMESIZE_RAW;
#else
        cdlastbuffer = (u8*)((u32)cdbuffer + ((curr_sector - cdlastsector) * CD_FRAMESIZE_RAW));
#endif
		return 0;
	}

	// Not found in cache...
    cdlastsector = curr_sector;
    cdlastbuffer = cdbuffer;
#ifdef SPMP_PSX4ALL
    spmp_cdr_cached_sectors = 0;
#endif

	if (!fmode) {
	        if (iscso) {
	          if (curr_sector >= cso->header.total_bytes / CD_FRAMESIZE_RAW) {
	            memset(cdlastbuffer, 0, CD_FRAMESIZE_RAW);
#ifdef SPMP_PSX4ALL
            spmp_cdr_cached_sectors = 1;
#endif
	          }
	          else {
                    int idx = cso->index[curr_sector] & 0x7fffffff;
                    int idx_end = cso->index[curr_sector + 1] & 0x7fffffff;
#ifdef SPMP_PSX4ALL
                    if (cso->header.align >= 16 ||
                        (unsigned)idx > (0x7fffffffu >> cso->header.align) ||
                        (unsigned)idx_end > (0x7fffffffu >> cso->header.align))
                        return -1;
                    idx <<= cso->header.align;
                    idx_end <<= cso->header.align;
                    if (idx_end < idx || idx_end-idx > CD_FRAMESIZE_RAW)
                        return -1;
#endif
                    if (cso->index[curr_sector] < 0) {
                      /* uncompressed sector */
                      fseek(cdHandle, idx, SEEK_SET);
#ifdef SPMP_PSX4ALL
                      if (fread(cdlastbuffer, 1, CD_FRAMESIZE_RAW, cdHandle) != CD_FRAMESIZE_RAW)
                          return -1;
#else
                      fread(cdlastbuffer, CD_FRAMESIZE_RAW, 1, cdHandle);
#endif
                      //DEBUGF("reading uncompressed sector %d (index %d)\n", curr_sector, idx);
                    }
                    else {
                      fseek(cdHandle, idx, SEEK_SET);
                      char buf[CD_FRAMESIZE_RAW];
#ifdef SPMP_PSX4ALL
                      if (fread(buf, 1, idx_end - idx, cdHandle) != (size_t)(idx_end - idx)) return -1;
#else
                      fread(buf, idx_end - idx, 1, cdHandle);
#endif
                      int x = uncompress2(cdlastbuffer, CD_FRAMESIZE_RAW, buf, idx_end - idx);
#ifdef SPMP_PSX4ALL
                      if (x != Z_OK) return -1;
#endif
                      //DEBUGF("uncompressing sector %d (index %d, size %d), return code %d\n", curr_sector, idx, idx_end - idx, x);
                      if (x) DEBUGF("error %d while uncompressing sector %lu\n", x, curr_sector);
                    }
#ifdef SPMP_PSX4ALL
                    spmp_cdr_cached_sectors = 1;
#endif
                  }
	        }
	        else {
          if (fseek(cdHandle, curr_sector * CD_FRAMESIZE_RAW, SEEK_SET) != 0) return -1;
#ifdef SPMP_PSX4ALL
          {
            size_t n = fread(cdlastbuffer, CD_FRAMESIZE_RAW, BUFFER_SECTORS, cdHandle);
            spmp_cdr_cached_sectors = (unsigned)n;
            if (!n) return -1;
            if (n < BUFFER_SECTORS)
                memset(cdlastbuffer+n*CD_FRAMESIZE_RAW, 0,
                       (BUFFER_SECTORS-n)*CD_FRAMESIZE_RAW);
          }
#else
          fread(cdlastbuffer, (CD_FRAMESIZE_RAW*BUFFER_SECTORS), 1, cdHandle);
#endif
		  //DEBUGF("reading sector %d\n", curr_sector);
                }
    } else if (fmode == 1) { //.Z
#ifdef SPMP_PSX4ALL
        spmp_cdr_cached_sectors = 1; /* .Z decompression: one sector. */
#endif
		int ret;
		for(i = 0; i <
#ifdef SPMP_PSX4ALL
            1
#else
            BUFFER_SECTORS
#endif
            ; i++)
		{
			u32 pos, p;
			u32 size;
			u8 Zbuf[CD_FRAMESIZE_RAW+256];

			p = curr_sector + i;

			pos = (u32)(Ztable[p * 6] | (Ztable[p * 6 + 1] << 8) | 
				(Ztable[p * 6 + 2] << 16) | (Ztable[p * 6 + 3] << 24));
			fseek(cdHandle, pos, SEEK_SET);

			p = (u16)(Ztable[p * 6 + 4] | (Ztable[p * 6 + 4 + 1] << 8));
      fread(Zbuf, p, 1, cdHandle);

			size = CD_FRAMESIZE_RAW;
			ret = uncompress(cdlastbuffer + (i * CD_FRAMESIZE_RAW), &size, Zbuf, p);
			if( ret != Z_OK )
			{
				printf("uncompress error %d !\n", ret);
				gp2x_deinit();
			}
		}
	} 
	else if (fmode == 2) 
	{ // .bz
		SysPrintf("BZ FORMAT IS NOT SUPPORTED.");
	}
    else { // .ZNX
#ifdef SPMP_PSX4ALL
        spmp_cdr_cached_sectors = 1; /* .ZNX decompression: one sector. */
#endif
		for(i = 0; i <
#ifdef SPMP_PSX4ALL
            1
#else
            BUFFER_SECTORS
#endif
            ; i++)
		{
			int ret;
			u32 pos, p;
			u32 size;
			u8 Zbuf[CD_FRAMESIZE_RAW+256];

			p = curr_sector + i;

			pos = (u32)(Ztable[p * 10] | (Ztable[p * 10 + 1] << 8) | (Ztable[p * 10 + 2] << 16) | (Ztable[p * 10 + 3] << 24));
			fseek(cdHandle, pos, SEEK_SET);

			p = (u16)(Ztable[p * 10 + 4] | (Ztable[p * 10 + 4 + 1] << 8));
      fread(Zbuf, p, 1, cdHandle);
			size = CD_FRAMESIZE_RAW;
			ret = uncompress(cdlastbuffer + (i * CD_FRAMESIZE_RAW), &size, Zbuf, p);
			if( ret != Z_OK )
			{
				printf("uncompress error %d !\n", ret);
				gp2x_deinit();
			}
		}
	}

	return 0;
}

// return the last read buffer
u8* CDR_getBuffer() {
	return (u8*)cdlastbuffer + 12;
}

// plays cdda audio
// sector : byte 0 - minute ; byte 1 - second ; byte 2 - frame
// does NOT uses bcd format
s32 CDR_play(u8 *sector) {
	return 0;
}

// stops cdda audio
s32 CDR_stop() {
	return 0;
}

s32 CDR_test() {
	if (*IsoFile == 0)
		return 0;
	cdHandle = fopen(IsoFile, "rb");
	if (cdHandle == NULL)
		return -1;
	fclose(cdHandle);
	cdHandle = NULL;
	return 0;
}

u8* CDR_getBufferSub() { return NULL; }

s32 CDR_getStatus(struct CdrStat* cdrstat) {
#if 0
    if (cdOpenCase) cdrstat->Status = 0x10;
    else		cdrstat->Status = 0;
#endif
	cdrstat->Status = 0;

    return 0;
}
