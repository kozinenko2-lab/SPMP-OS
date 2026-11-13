/*  Pcsx - Pc Psx Emulator
 *  Copyright (C) 1999-2003  Pcsx Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>

#include "common.h"
#ifdef SPMP_PSX4ALL
#include "spmp_port/spmp_psx_bios.h"
#endif

uintptr_t *psxMemWLUT = NULL;
uintptr_t *psxMemRLUT = NULL;
s8 *psxM = NULL;
s8 *psxP = NULL;
s8 *psxR = NULL;
s8 *psxH = NULL;
s8 *psxNULLread=NULL;

/* Cache-isolation state must not leak into the next loaded ROM. */
static int spmp_psx_mem_write_enabled = 1;

int psxMemInit() {
	int i;
    if (psxM || psxP || psxR || psxH || psxNULLread || psxMemRLUT || psxMemWLUT) return -1;
    spmp_psx_mem_write_enabled = 1;

	psxMemRLUT = (uintptr_t*)malloc(0x10000 * sizeof(*psxMemRLUT));
	psxMemWLUT = (uintptr_t*)malloc(0x10000 * sizeof(*psxMemWLUT));
    if (!psxMemRLUT || !psxMemWLUT) {
        free(psxMemRLUT); free(psxMemWLUT);
        psxMemRLUT = psxMemWLUT = NULL;
        return -1;
    }
    memset(psxMemRLUT, 0, 0x10000 * sizeof(*psxMemRLUT));
    memset(psxMemWLUT, 0, 0x10000 * sizeof(*psxMemWLUT));

#ifdef SPMP_PSX4ALL
    /* The SPMP OS arena/heap provides RAM, not Linux MAP_FIXED. */
    psxM = (s8*)malloc(0x00200000);
#else
    psxM = (s8*)mmap((void*)0x10000000, 0x200000, PROT_READ|PROT_WRITE,
                        MAP_SHARED|MAP_FIXED|MAP_ANONYMOUS, -1, 0);
#endif
	//DEBUGF("RAM mmapped at %p", psxM);
	psxP = (s8*)malloc(0x00010000);
	psxH = (s8*)malloc(0x00010000);
	psxR = (s8*)malloc(0x00080000);
	
	psxNULLread=(s8*)malloc(0x00010000);
    if (psxNULLread) memset(psxNULLread, 0, 0x10000);
	
    if (psxM == NULL ||
#ifndef SPMP_PSX4ALL
        psxM == (s8*)MAP_FAILED ||
#endif
        psxP == NULL || psxH == NULL || psxR == NULL || psxNULLread == NULL) {
        SysPrintf("Error allocating memory");
        psxMemShutdown();
        psxM=psxP=psxR=psxH=psxNULLread=NULL;
        psxMemRLUT=psxMemWLUT=NULL;
        return -1;
	}

// MemR
	for (i=0; i<0x10000; i++) psxMemRLUT[i]=(uintptr_t)psxNULLread; /* Never point LUT at address of pointer variable */
	for (i=0; i<0x80; i++) psxMemRLUT[i + 0x0000] = (uintptr_t)&psxM[(i & 0x1f) << 16];
	memcpy(psxMemRLUT + 0x8000, psxMemRLUT, 0x80 * sizeof(*psxMemRLUT));
	memcpy(psxMemRLUT + 0xa000, psxMemRLUT, 0x80 * sizeof(*psxMemRLUT));

	psxMemRLUT[0x1f00] = (uintptr_t)psxP;
	psxMemRLUT[0x1f80] = (uintptr_t)psxH;

	for (i=0; i<0x08; i++) psxMemRLUT[i + 0xbfc0] = (uintptr_t)&psxR[i << 16];

// MemW
	for (i=0; i<0x80; i++) psxMemWLUT[i + 0x0000] = (uintptr_t)&psxM[(i & 0x1f) << 16];
	memcpy(psxMemWLUT + 0x8000, psxMemWLUT, 0x80 * sizeof(*psxMemWLUT));
	memcpy(psxMemWLUT + 0xa000, psxMemWLUT, 0x80 * sizeof(*psxMemWLUT));

	psxMemWLUT[0x1f00] = (uintptr_t)psxP;
	psxMemWLUT[0x1f80] = (uintptr_t)psxH;

	return 0;
}

void psxMemReset() {
    if (!psxM || !psxP || !psxR) return;
    memset(psxM, 0, 0x00200000);
    memset(psxP, 0, 0x00010000);
    if (Config.HLE) { memset(psxR,0,0x80000); return; }
#ifdef SPMP_PSX4ALL
    char full_path[600];
    spmp_psx_bios_info metadata;
    spmp_psx_bios_status result=SPMP_PSX_BIOS_PATH_TOO_LONG;
    if (spmp_psx_bios_make_path(full_path,sizeof(full_path),
                                (const char *)Config.BiosDir,(const char *)Config.Bios))
        result=spmp_psx_bios_load(full_path,(uint8_t *)psxR,0x80000,&metadata);
    if(result!=SPMP_PSX_BIOS_OK) {
        SysPrintf("SPMP BIOS load failed (%d): %s; switching to HLE\n",
                   (int)result, spmp_psx_bios_status_text(result));
        Config.HLE=1;
        memset(psxR,0,0x80000);
    } else {
        SysPrintf("SPMP BIOS ready: %s / region %c / crc32 %08x\n",
                   metadata.version,metadata.region,metadata.crc32);
    }
#else
    FILE *file=NULL;
    char full_path[600];
    snprintf(full_path,sizeof(full_path),"%s%s",Config.BiosDir,Config.Bios);
    file=fopen(full_path,"rb");
    if(!file || fread(psxR,1,0x80000,file)!=0x80000) {
        SysPrintf("BIOS NOT FOUND: %s",full_path);
        memset(psxR,0,0x80000);
        Config.HLE=1;
    }
    if(file) fclose(file);
#endif
}

void psxMemShutdown() {
	if( psxNULLread != NULL ) free(psxNULLread);
#ifdef SPMP_PSX4ALL
    if (psxM) free(psxM);
#else
    if (psxM && psxM!=(s8*)MAP_FAILED) munmap(psxM, 0x200000);
#endif
	if( psxP != NULL ) free(psxP);
	if( psxH != NULL ) free(psxH);
	if( psxR != NULL ) free(psxR);
	if( psxMemRLUT != NULL ) free(psxMemRLUT);
	if( psxMemWLUT != NULL ) free(psxMemWLUT);
    psxM = psxP = psxR = psxH = psxNULLread = NULL;
    psxMemRLUT = psxMemWLUT = NULL;
    spmp_psx_mem_write_enabled = 1;
}

void psxMemWrite32_error(u32 mem, u32 value)
{
	/* Shared lifecycle state resets on shutdown/re-init. */
	if (mem==0xfffe0130)
	{
		switch (value) {
			case 0x800:
			case 0x804:
				if (spmp_psx_mem_write_enabled == 0) break;
				spmp_psx_mem_write_enabled = 0;
				memset(psxMemWLUT + 0x0000, 0, 0x80 * sizeof(*psxMemWLUT));
				memset(psxMemWLUT + 0x8000, 0, 0x80 * sizeof(*psxMemWLUT));
				memset(psxMemWLUT + 0xa000, 0, 0x80 * sizeof(*psxMemWLUT));
				break;
			case 0x1e988:
				if (spmp_psx_mem_write_enabled == 1) break;
				spmp_psx_mem_write_enabled = 1;
				{ int i; for (i=0; i<0x80; i++) psxMemWLUT[i + 0x0000] = (uintptr_t)&psxM[(i & 0x1f) << 16]; }
				memcpy(psxMemWLUT + 0x8000, psxMemWLUT, 0x80 * sizeof(*psxMemWLUT));
				memcpy(psxMemWLUT + 0xa000, psxMemWLUT, 0x80 * sizeof(*psxMemWLUT));
				break;
		}
	}
#ifdef DYNAREC
	else if (!spmp_psx_mem_write_enabled) REC_CLEARM(mem);
#endif	
}


s8 psxMemReadS8(u32 mem) {
	u32 t=mem>>16;
	u32 m=mem&0xFFFF;
	if (t!=0x1f80)
		return (*(((s8*)psxMemRLUT[t])+m));
	else if (m<0x1000)
		return (*(((s8*)psxH)+m));
	else
		return (s8)psxHwRead8(mem);
}

u8 psxMemRead8(u32 mem) {
	u32 t=mem>>16;
	u32 m=mem&0xFFFF;
	if (t!=0x1f80)
		return (*(((u8*)psxMemRLUT[t])+m));
	else if (m<0x1000)
		return (*(((u8*)psxH)+m));
	else
		return (u8)psxHwRead8(mem);
}

s16 psxMemReadS16(u32 mem) {
	u32 t=mem>>16;
	u32 m=mem&0xFFFF;
	if (t!=0x1f80)
		return SWAPu16(*((s16*)(((s8*)psxMemRLUT[t])+m)));
	else if (m<0x1000)
		return (s16)(SWAP16(*((u16*)(((u8*)psxH)+m))));
	else
		return (s16)psxHwRead16(mem);
}

u16 psxMemRead16(u32 mem) {
	u32 t=mem>>16;
	u32 m=mem&0xFFFF;
	if (t!=0x1f80)
		return SWAPu16(*((u16*)(((u8*)psxMemRLUT[t])+m)));
	else if (m<0x1000)
		return SWAP16(*((u16*)(((u8*)psxH)+m)));
	else
		return (u16)psxHwRead16(mem);
}

u32 psxMemRead32(u32 mem) {
	u32 t=mem>>16;
	u32 m=mem&0xFFFF;
	if (t!=0x1f80)
		return SWAPu32(*((u32*)(((u8*)psxMemRLUT[t])+m)));
	else if (m<0x1000)
		return SWAP32(*((u32*)(((u8*)psxH)+m)));
	else
		return (u32)psxHwRead32(mem);
}

void psxMemWrite8(u32 mem, u32 value) {
	//DEBUGG("write8 mem 0x%x val 0x%x pc 0x%x\n", mem, value, psxRegs->pc);
	u32 t=mem>>16;
	u32 m=mem&0xFFFF;
	if (t!=0x1f80)
	{
		u8 *p=(u8 *)(psxMemWLUT[t]);
		if (p) {
			*(p+m)=(u8)value;
#ifdef DYNAREC
			REC_CLEARM(mem&(~3));
#endif
		}
	}
	else if (m<0x1000)
		*(((u8*)psxH)+m)=(u8)value;
	else
		psxHwWrite8(mem, value);
}


void psxMemWrite16(u32 mem, u32 value) {
	//DEBUGG("write16 mem 0x%x val 0x%x pc 0x%x\n", mem, value, psxRegs->pc);
	u32 t=mem>>16;
	u32 m=mem&0xFFFF;
	if (t!=0x1f80)
	{
		u8 *p=(u8 *)(psxMemWLUT[t]);
		if (p) {
			*((u16*)(p+m))=SWAPu16(value);
#ifdef DYNAREC
			REC_CLEARM(mem&(~3));
#endif
		}
	}
	else if (m<0x1000)
		*((u16*)(((u8*)psxH)+m))=(u16)SWAPu16(value);
	else
		psxHwWrite16(mem, value);
}

void psxMemWrite32(u32 mem, u32 value) {
	//DEBUGG("write32 mem 0x%x val 0x%x pc 0x%x\n", mem, value, psxRegs->pc);
	u32 t=mem>>16;
	u32 m=mem&0xFFFF;
	if (t!=0x1f80)
	{
		u8 *p=(u8 *)(psxMemWLUT[t]);
		if (p) {
			*((u32*)(p+m))=SWAPu32(value);
#ifdef DYNAREC
			REC_CLEARM(mem&(~3));
#endif
		} else
			psxMemWrite32_error(mem,value);
	}
	else if (m<0x1000)
		*((u32*)(((u8*)psxH)+m))=(u32)SWAPu32(value);
	else
		psxHwWrite32(mem, value);
}
