#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cstdarg>
#include <vector>
#include <string>
#include <unistd.h>
#include <zlib.h>
#include "../vendor/psx4all-dingoo-master/src/common.h"
#include "../vendor/psx4all-dingoo-master/src/cdr/oldversion/cdriso.h"
s8* packfile = nullptr;
void SysPrintf(const char *fmt, ...) { (void)fmt; }
void gp2x_deinit(void) { abort(); }
static int checks=0;
#define REQ(x) do { if(!(x)){ fprintf(stderr,"CDR FAIL line %d: %s\n",__LINE__,#x);exit(1);} ++checks;}while(0)
static std::string tempfile(const char *ext) {
    char path[]="/tmp/spmp_psx_cdXXXXXX";
    int fd=mkstemp(path);REQ(fd>=0);close(fd);
    std::string name=std::string(path)+ext;
    REQ(rename(path,name.c_str())==0);
    return name;
}
static void put32(FILE *f,uint32_t v) { unsigned char b[4]={static_cast<unsigned char>(v),static_cast<unsigned char>(v>>8),static_cast<unsigned char>(v>>16),static_cast<unsigned char>(v>>24)};REQ(fwrite(b,1,4,f)==4); }
static void write_iso(FILE *f) {
    unsigned char buf[2352];
    for(unsigned n=0;n<3;n++) {
      memset(buf,0,sizeof(buf));
      for(unsigned i=0;i<2352;i++)buf[i]=(unsigned char)((n*71+i*3)&255);
      REQ(fwrite(buf,1,sizeof(buf),f)==sizeof(buf));
    }
}
static void write_cso_compressed(FILE* f) {
    unsigned char raw[2352];
    std::vector<unsigned char> compressed;
    std::vector<uint32_t> offsets;
    for(int n=0;n<3;n++) {
        memset(raw,0,sizeof(raw));
        for(unsigned i=0;i<2352;i++)raw[i]=(unsigned char)((n*71+i*3)&255);
        z_stream stream={};
        REQ(deflateInit2(&stream,9,Z_DEFLATED,-15,8,Z_DEFAULT_STRATEGY)==Z_OK);
        unsigned char tmp[2400];
        stream.next_in=raw;stream.avail_in=2352;stream.next_out=tmp;stream.avail_out=sizeof(tmp);
        REQ(deflate(&stream,Z_FINISH)==Z_STREAM_END);
        offsets.push_back((uint32_t)(40+compressed.size()));
        compressed.insert(compressed.end(),tmp,tmp+stream.total_out);
        REQ(deflateEnd(&stream)==Z_OK);
    }
    offsets.push_back((uint32_t)(40+compressed.size()));
    REQ(fwrite("CISO",1,4,f)==4);
    put32(f,0);put32(f,3*2352);put32(f,0);put32(f,2352);
    REQ(fputc(1,f)!=EOF);REQ(fputc(0,f)!=EOF);REQ(fputc(0,f)!=EOF);REQ(fputc(0,f)!=EOF);
    for(uint32_t offset:offsets)put32(f,offset);
    REQ(fwrite(compressed.data(),1,compressed.size(),f)==compressed.size());
}
static void read_test_sectors(std::string path) {
    packfile=(s8*)path.c_str();
    REQ(CDR_open()==0);
    for(int sector=0;sector<3;sector++) {
       u8 time[3]={0,2,(u8)sector};REQ(CDR_readTrack(time)==0);
       unsigned char *data=CDR_getBuffer();
       for(int off=0;off<80;off++) REQ(data[off]==(uint8_t)((sector*71+(off+12)*3)&255));
    }
    {u8 end[3]={0,2,3}; int rc=CDR_readTrack(end);
      if (path.find(".cbn") != std::string::npos) {
        REQ(rc==0); REQ(CDR_getBuffer()[0]==0); /* CISO lead-out is zero-filled */
      } else REQ(rc!=0);
    }
    REQ(CDR_close()==0);
}
int main() {
    std::string iso=tempfile(".bin");
    FILE *f=fopen(iso.c_str(),"wb");REQ(f!=nullptr);write_iso(f);REQ(!fclose(f));
    read_test_sectors(iso);
    REQ(unlink(iso.c_str())==0);
    std::string cso=tempfile(".cbn");
    f=fopen(cso.c_str(),"wb");REQ(f!=nullptr);
    REQ(fwrite("CISO",1,4,f)==4);
    put32(f,0);put32(f,3*2352);put32(f,0);put32(f,2352);
    REQ(fputc(1,f)!=EOF);REQ(fputc(0,f)!=EOF);REQ(fputc(0,f)!=EOF);REQ(fputc(0,f)!=EOF);
    for(uint32_t i=0;i<4;i++)put32(f,24+16+i*2352+(i<3?0x80000000u:0));
    write_iso(f);REQ(!fclose(f));
    read_test_sectors(cso);
    REQ(unlink(cso.c_str())==0);
    std::string compressed=tempfile(".cbn");
    f=fopen(compressed.c_str(),"wb");REQ(f!=nullptr);
    write_cso_compressed(f); REQ(!fclose(f));
    read_test_sectors(compressed);
    REQ(unlink(compressed.c_str())==0);
    printf("PSX4ALL CDR: %d actual read checks passed (raw / CISO uncompressed / CISO deflate)\n",checks);
}
