/* Reconstructed MMPi checksum and record traversal from original RedBoot.
 * Evidence: variant 3 at 0x3e48c, 0x3e734, 0x3e3f0.
 * Host-only inspector. Bounds checks below are added; NO target memory writes.
 * Not an OEM source file, target loader, or replacement firmware.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint32_t le32(const unsigned char *p) {
    return (uint32_t)p[0] | (uint32_t)p[1]<<8 |
           (uint32_t)p[2]<<16 | (uint32_t)p[3]<<24;
}
/* RedBoot divides payload into four runs: q, q, q, remaining words.
 * Sums 0+2 and 1+3, modulo 2^32. These are NOT cryptographic hashes.
 */
static void mmpi_sums(const unsigned char *p, size_t bytes, uint32_t sums[2]) {
    size_t q=bytes/16, cursor=0;
    sums[0]=sums[1]=0;
    for (size_t run=0; run<4; ++run) {
        size_t words=q + (run==3 ? (bytes%16)/4 : 0);
        for (size_t i=0; i<words; ++i, cursor+=4)
            sums[run&1] += le32(p+cursor);
    }
}
static int inspect(const unsigned char *p,size_t len) {
    if (len<16 || memcmp(p,"MMPi",4)!=0 || le32(p+4)!=len || (len-16)%4) {
        fputs("invalid MMPi header/length/alignment\n",stderr); return 1;
    }
    uint32_t sums[2]; mmpi_sums(p+16,len-16,sums);
    if (sums[0]!=le32(p+8) || sums[1]!=le32(p+12)) {
        fputs("MMPi checksum mismatch\n",stderr); return 1;
    }
    printf("checksums %08x %08x\n",sums[0],sums[1]);
    size_t at=16; unsigned descriptors=0;
    while (at<len) {
        if (len-at<36 || memcmp(p+at,"MMPd",4)!=0 || le32(p+at+4)!=36) {
            fputs("unsupported/invalid MMPd descriptor\n",stderr);return 1;
        }
        uint32_t flags=le32(p+at+16),count=le32(p+at+20),entry=le32(p+at+24);
        printf("MMPd %08zx flags %08x count %u entry %08x\n",at,flags,count,entry);
        ++descriptors; at+=36;
        for (uint32_t i=0;i<count;++i) {
            if (len-at<24 || memcmp(p+at,"MMPs",4)!=0) {
                fputs("invalid MMPs header\n",stderr);return 1;
            }
            uint32_t n=le32(p+at+4),address=le32(p+at+8);
            if (n<24 || n>len-at || n%4) {
                fputs("invalid MMPs length\n",stderr);return 1;
            }
            printf("MMPs %08zx destination %08x bytes %u\n",at,address,n-24);
            at+=n;
        }
    }
    if (!descriptors) {fputs("no descriptors\n",stderr);return 1;}
    return 0;
}
int main(int argc,char **argv) {
    if (argc!=2) {fprintf(stderr,"usage: %s original.RAP\n",argv[0]);return 2;}
    FILE *f=fopen(argv[1],"rb");
    if (!f) {perror("open");return 2;}
    if (fseek(f,0,SEEK_END)!=0) {fclose(f);return 2;}
    long size=ftell(f);
    if (size<0 || size>64*1024*1024 || fseek(f,0,SEEK_SET)!=0) {fclose(f);return 2;}
    unsigned char *p=malloc(size ? (size_t)size : 1);
    if (!p) {fclose(f);return 2;}
    if (fread(p,1,(size_t)size,f)!=(size_t)size) {free(p);fclose(f);return 2;}
    fclose(f);int rc=inspect(p,(size_t)size);free(p);return rc;
}
