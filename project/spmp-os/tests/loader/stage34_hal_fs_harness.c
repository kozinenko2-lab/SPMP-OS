#include "spmp_ro_fs.h"
#include "spmp_diag_hal.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef struct{int trust,diag_callbacks,fs_callbacks,dir_calls;} mock;
static int trust(void *p){return ((mock*)p)->trust;}
static int keys(void *p,uint32_t *v){((mock*)p)->diag_callbacks++;*v=0x1234u;return 0;}
static int ticks(void *p,uint32_t *v){((mock*)p)->diag_callbacks++;*v=347u;return 0;}
static int sd(void *p,int *v){((mock*)p)->diag_callbacks++;*v=1;return 0;}
static int lcd(void *p,uint16_t *w,uint16_t *h){((mock*)p)->diag_callbacks++;*w=320;*h=240;return 0;}
static int audio(void *p,uint32_t *hz,uint8_t *ch){((mock*)p)->diag_callbacks++;*hz=22050;*ch=2;return 0;}
static int fs_confirm(void *p){((mock*)p)->fs_callbacks++;return ((mock*)p)->trust;}
static int read_dir(void *p,const char *path,size_t skip,spmp_ro_entry *e,size_t cap,
  size_t *written,size_t *total,size_t *files){
    mock *m=p;m->dir_calls++;assert(path[0]=='/' && cap==SPMP_FS_MAX_ENTRIES);
    *total=150;*files=148;
    *written=150-skip<cap?150-skip:cap;
    for(size_t i=0;i<*written;i++){
      memset(&e[i],0,sizeof e[i]);
      snprintf(e[i].name,sizeof e[i].name,"file%03zu.txt",skip+i);
      e[i].bytes=(skip+i)*128u;e[i].is_dir=(skip+i<2);
      e[i].fat_date=(uint16_t)(((2026u-1980u)<<9)|(11u<<5)|11u);
    }
    return 0;
}
int main(void){
    mock m={0};spmp_diag_hal diag={&m,keys,ticks,sd,lcd,audio,trust};
    spmp_diag_snapshot s;
    assert(spmp_diag_readonly_snapshot(&diag,&s)!=0);
    assert(m.diag_callbacks==0);
    m.trust=1;
    assert(spmp_diag_readonly_snapshot(&diag,&s)==0);
    assert(s.available==31 && !s.failed && s.keys==0x1234 && s.milliseconds==347);
    assert(s.sd_present && s.lcd_width==320 && s.lcd_height==240);
    assert(s.sample_rate_hz==22050 && s.audio_channels==2);
    assert(spmp_ro_path_valid("/") && spmp_ro_path_valid("/GBA/Russian Test.gba"));
    const char *bad[]={"../abc","/../abc","/dir/../abc","/dir//abc","/abc/","/a\\b","/a:b","","relative","/dir/..","/dir/./.."};
    for(unsigned i=0;i<sizeof bad/sizeof bad[0];i++)assert(!spmp_ro_path_valid(bad[i]));
    spmp_ro_fs fs={&m,fs_confirm,read_dir};
    spmp_ro_panel *panels=calloc(3,sizeof(spmp_ro_panel));assert(panels);
    spmp_ro_panel *first=panels,*second=panels+1,*scratch=panels+2;
    assert(spmp_ro_open_page(&fs,first,scratch,"/GAMES",0)==0);
    assert(first->count==64 && first->total_entries==150 && first->total_files==148);
    assert(first->entries[0].bytes==0);
    assert(spmp_ro_open_page(&fs,second,scratch,"/GAMES",128)==0);
    assert(second->count==22 && second->entries[0].bytes==128*128u);
    char date[9];assert(spmp_ro_short_date(first->entries[0].fat_date,date)==0);
    assert(!strcmp(date,"11.11.26"));
    spmp_ro_dual_panel *dual=calloc(1,sizeof(*dual));assert(dual);
    *(&dual->panels[0])=*first;*(&dual->panels[1])=*second;
    assert(spmp_ro_switch(dual,1)==0 && spmp_ro_select(dual,21)==0);
    assert(dual->panels[1].selected==21);
    assert(spmp_ro_select(dual,22)!=0 && spmp_ro_switch(dual,2)!=0);
    m.trust=0;
    char oldpath[SPMP_FS_PATH_MAX];strcpy(oldpath,second->path);
    assert(spmp_ro_open_page(&fs,second,scratch,"/",0)!=0);
    assert(!strcmp(second->path,oldpath));
    assert(m.fs_callbacks==3 && m.dir_calls==2);
    puts("Stage34 HAL/FS: 5 read-only probes, two panels, paged 150 entries, FAT date, fail-closed paths PASS");
    free(dual);free(panels);return 0;
}
