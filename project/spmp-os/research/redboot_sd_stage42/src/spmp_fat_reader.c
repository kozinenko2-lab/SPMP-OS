/* SPMP-OS Stage 42: conservative read-only FAT16/FAT32 loader. No standard
 * library dependencies; all I/O enters via a fail-bounded callback.
 * This is not an SD/MMC controller driver and is NOT a flashable binary. */
#include "spmp_fat_reader.h"

typedef struct {
    const spmp_fat_io *io;
    uint32_t reads;
    uint32_t first, volume_sectors;
    uint32_t fat_begin, fat_sectors, root_begin, root_sectors;
    uint32_t data_begin, cluster_count, root_cluster;
    uint8_t sectors_per_cluster, fat32;
    uint8_t block[SPMP_FAT_SECTOR_BYTES];
} vol;
static uint16_t le16(const uint8_t *p) {return (uint16_t)(p[0]|((uint16_t)p[1]<<8));}
static uint32_t le32(const uint8_t *p) {
    return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
}
static int power2(uint32_t x) {return x && !(x&(x-1u));}
static int readblk(vol *v, uint64_t block) {
    if(block>=v->io->physical_sectors || block>0xffffffffu ||
       v->reads>=v->io->max_reads) return 0;
    v->reads++;
    return v->io->read_sector(v->io->context,(uint32_t)block,v->block)==1;
}
static int check_range(uint64_t start,uint64_t size,uint64_t limit) {
    return size && start<limit && size<=limit-start;
}
static int parse_volume(vol *v, uint32_t at, uint32_t limit) {
    uint32_t res, fats, fatsz, roots, all, spc, rsectors, data, clusters;
    uint64_t fat0, root0, data0;
    uint8_t *b=v->block;
    if(!readblk(v,at)) return 0;
    if(b[510]!=0x55||b[511]!=0xaa||le16(b+11)!=512) return 0;
    spc=b[13];res=le16(b+14);fats=b[16];roots=le16(b+17);
    all=le16(b+19);if(!all) all=le32(b+32);
    fatsz=le16(b+22);if(!fatsz) fatsz=le32(b+36);
    if(!power2(spc)||spc>128||!res||!fats||fats>2||!fatsz||
       !all||all>limit||at>v->io->physical_sectors||
       all>v->io->physical_sectors-at) return 0;
    rsectors=((uint64_t)roots*32+511)/512;
    if((uint64_t)res+(uint64_t)fats*fatsz+rsectors>=all) return 0;
    data=all-(uint32_t)((uint64_t)res+(uint64_t)fats*fatsz+rsectors);
    clusters=data/spc;
    if(clusters<4085||clusters>=0x0ffffff5u) return 0; /* reject FAT12 */
    v->fat32=(uint8_t)(clusters>=65525);
    if((v->fat32 && (roots!=0 || le16(b+22)!=0 || le16(b+42)!=0)) ||
       (!v->fat32 && (roots==0 || le16(b+22)==0))) return 0;
    if((uint64_t)fatsz*512 < (uint64_t)(clusters+2u)*(v->fat32?4u:2u)) return 0;
    if(v->fat32) {
        uint16_t flags=le16(b+40);
        uint32_t active=(flags&0x80)?(flags&15u):0u;
        if(active>=fats) return 0;
        fat0=(uint64_t)at+res+(uint64_t)active*fatsz;
        v->root_cluster=le32(b+44)&0x0fffffffu;
        if(v->root_cluster<2||v->root_cluster-2u>=clusters) return 0;
    } else {fat0=(uint64_t)at+res;v->root_cluster=0;}
    root0=(uint64_t)at+res+(uint64_t)fats*fatsz;
    data0=root0+rsectors;
    if(fat0>0xffffffffu || root0>0xffffffffu || data0>0xffffffffu) return 0;
    v->first=at;v->volume_sectors=all;v->fat_begin=(uint32_t)fat0;
    v->fat_sectors=fatsz;v->root_begin=(uint32_t)root0;
    v->root_sectors=rsectors;v->data_begin=(uint32_t)data0;
    v->cluster_count=clusters;v->sectors_per_cluster=(uint8_t)spc;
    return 1;
}
static int open_volume(vol *v) {
    uint32_t i, first=0, n=0;
    /* Superfloppy first, otherwise a single supported MBR partition. */
    if(parse_volume(v,0,v->io->physical_sectors)) return 1;
    if(!readblk(v,0)) return 0;
    if(v->block[510]!=0x55||v->block[511]!=0xaa) return 0;
    for(i=0;i<4;i++) {
        const uint8_t *p=v->block+446+16*i;
        uint8_t type=p[4];
        if(type==0x04||type==0x06||type==0x0e||type==0x0b||type==0x0c){
            first=le32(p+8);n=le32(p+12);break;
        }
    }
    if(!first||!check_range(first,n,v->io->physical_sectors)) return 0;
    return parse_volume(v,first,n);
}
static int cluster_valid(const vol *v,uint32_t c) {
    return c>=2 && c-2u<v->cluster_count;
}
/* -1=I/O error / invalid cluster, 0=end-of-chain, 1=next cluster. */
static int next_cluster(vol *v, uint32_t cluster,uint32_t *next) {
    uint64_t offset,sector;
    uint32_t value;
    if(!cluster_valid(v,cluster)) return -1;
    offset=(uint64_t)cluster*(v->fat32?4u:2u);
    sector=offset/512;
    if(sector>=v->fat_sectors||!readblk(v,(uint64_t)v->fat_begin+sector)) return -1;
    value=v->fat32?(le32(v->block+(offset%512))&0x0fffffffu):le16(v->block+(offset%512));
    if(value>=(v->fat32?0x0ffffff8u:0xfff8u)) return 0;
    if(!cluster_valid(v,value)) return -1;
    *next=value;return 1;
}
static int cluster_sector(const vol *v,uint32_t c,uint32_t s,uint32_t *lba) {
    uint64_t rel,abs;
    if(!cluster_valid(v,c)||s>=v->sectors_per_cluster) return 0;
    rel=(uint64_t)(c-2u)*v->sectors_per_cluster+s;
    abs=(uint64_t)v->data_begin+rel;
    if(abs>0xffffffffu||abs>=(uint64_t)v->first+v->volume_sectors) return 0;
    *lba=(uint32_t)abs;return 1;
}
static int match_name(const uint8_t *entry,const uint8_t name[11]) {
    unsigned i;for(i=0;i<11;i++)if(entry[i]!=name[i])return 0;return 1;
}
static const uint8_t DIRNAME[11]={'S','P','M','P','O','S',' ',' ',' ',' ',' '};
static const uint8_t FILENAME[11]={'B','O','O','T',' ',' ',' ',' ','P','K','G'};
static int scan_entries(vol *v,const uint8_t want[11],int want_dir,
                        uint32_t *cluster,uint32_t *size,int *end) {
    uint32_t pos;
    for(pos=0;pos<512;pos+=32) {
        const uint8_t *e=v->block+pos;
        uint8_t attr=e[11];
        if(e[0]==0){*end=1;return 0;}
        if(e[0]==0xe5||attr==0x0f||(attr&0x08))continue;
        if((!!(attr&0x10))!=want_dir)continue;
        if(match_name(e,want)) {
            uint32_t c=(uint32_t)le16(e+26);
            if(v->fat32)c|=(uint32_t)le16(e+20)<<16;
            if(!cluster_valid(v,c))return -1;
            *cluster=c;*size=le32(e+28);return 1;
        }
    }
    return 0;
}
static int find_entry(vol *v,uint32_t dir,int root16,const uint8_t name[11],
                      int want_dir,uint32_t *cluster,uint32_t *size) {
    uint32_t i,j,current=dir,steps=0;
    if(root16) {
        for(i=0;i<v->root_sectors;i++) {
            int end=0,r;
            if(!readblk(v,(uint64_t)v->root_begin+i))return 0;
            r=scan_entries(v,name,want_dir,cluster,size,&end);
            if(r==1)return 1;if(r<0||end)return 0;
        }
        return 0;
    }
    while(cluster_valid(v,current) && steps++<v->cluster_count) {
        for(j=0;j<v->sectors_per_cluster;j++) {
            int end=0,r;uint32_t addr;
            if(!cluster_sector(v,current,j,&addr)||!readblk(v,addr))return 0;
            r=scan_entries(v,name,want_dir,cluster,size,&end);
            if(r==1)return 1;if(r<0||end)return 0;
        }
        int r=next_cluster(v,current,&current);
        if(r!=1)return 0;
    }
    return 0;
}
static int read_file(vol *v,uint32_t cluster,uint8_t *out,size_t count,size_t *copied) {
    uint32_t steps=0,current=cluster;
    *copied=0;
    while(*copied<count&&steps++<v->cluster_count) {
        uint32_t j;
        if(!cluster_valid(v,current))return 0;
        for(j=0;j<v->sectors_per_cluster&&*copied<count;j++) {
            uint32_t sector;size_t take=count-*copied,k;
            if(!cluster_sector(v,current,j,&sector)||!readblk(v,sector))return 0;
            if(take>512)take=512;
            for(k=0;k<take;k++)out[*copied+k]=v->block[k];
            *copied+=take;
        }
        if(*copied==count) {
            /* For boot artifacts demand a clean, exact cluster chain: this
             * rejects cycles, hidden tail clusters, and damaged FAT links. */
            uint32_t ignored;
            return next_cluster(v,current,&ignored)==0;
        }
        if(next_cluster(v,current,&current)!=1)return 0;
    }
    return 0;
}
int spmp_fat_read_boot(const spmp_fat_io *io,uint8_t *out,size_t capacity,size_t *actual) {
    vol v={0};uint32_t folder=0,file=0,ignored=0,size=0;size_t copied=0;
    int success=0;
    if(actual)*actual=0;
    if(!io||!actual||!out||!io->read_sector||!io->physical_sectors||
       !io->max_reads||capacity<48u+52u||capacity>SPMP_FAT_MAX_PACKAGE)
        return 0;
    v.io=io;
    if(!open_volume(&v))goto done;
    if(!find_entry(&v,v.root_cluster,!v.fat32,DIRNAME,1,&folder,&ignored))goto done;
    if(!find_entry(&v,folder,0,FILENAME,0,&file,&size))goto done;
    if(size<48u+52u||size>SPMP_FAT_MAX_PACKAGE||size>capacity)goto done;
    if(!read_file(&v,file,out,size,&copied))goto done;
    *actual=size;success=1;
done:
    if(!success){size_t j;for(j=0;j<copied;j++)out[j]=0;}
    return success;
}
