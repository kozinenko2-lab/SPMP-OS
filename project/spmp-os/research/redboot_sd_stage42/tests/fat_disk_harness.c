#include "spmp_fat_reader.h"
#include "spmp_dualboot.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct {FILE *f;long count,fail_on; } IO;
static int sector(void *ptr,uint32_t lba,uint8_t out[512]) {
    IO *io=(IO*)ptr;io->count++;
    if(io->fail_on&&io->count==io->fail_on)return 0;
    if(fseek(io->f,(long)lba*512,SEEK_SET))return 0;
    return fread(out,1,512,io->f)==512;
}
int main(int argc,char **argv) {
    uint8_t *output;size_t count=0; long len;IO io={0};spmp_fat_io fi={0};int ok,expected,check_package;
    if(argc!=6){fprintf(stderr,"usage: harness disk.img expected[0|1] budget fail_on verify_pkg[0|1]\n");return 2;}
    io.f=fopen(argv[1],"rb");if(!io.f){perror("disk");return 2;}
    fseek(io.f,0,SEEK_END);len=ftell(io.f);
    if(len<512||len%512){fclose(io.f);return 2;}
    fi.context=&io;fi.read_sector=sector;fi.physical_sectors=(uint32_t)(len/512);
    fi.max_reads=(uint32_t)strtoul(argv[3],NULL,10);
    io.fail_on=strtol(argv[4],NULL,10);expected=atoi(argv[2]);check_package=atoi(argv[5]);
    output=malloc(SPMP_FAT_MAX_PACKAGE);if(!output){fclose(io.f);return 2;}
    memset(output,0xaa,SPMP_FAT_MAX_PACKAGE);
    ok=spmp_fat_read_boot(&fi,output,SPMP_FAT_MAX_PACKAGE,&count);
    if(ok!=expected||(!ok&&count)){fprintf(stderr,"FAILED got=%d expect=%d bytes=%zu reads=%ld\n",ok,expected,count,io.count);return 1;}
    if(ok&&check_package) {
      const uint8_t *elf=NULL;size_t elfsize=0;
      if(!spmp_db_verify_package(output,count,0x00420001,&elf,&elfsize)){
        fprintf(stderr,"Package failed Stage41 verifier\n");return 1;
      }
    }
    printf("PASS ok=%d bytes=%zu sector_reads=%ld\n",ok,count,io.count);
    free(output);fclose(io.f);return 0;
}
