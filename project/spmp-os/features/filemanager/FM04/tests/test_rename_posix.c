/* FM04 real rename through FM01 POSIX scanner and FM02 atomic rename backend. */
#define _GNU_SOURCE
#include "spmp_fm04.h"
#include "fm01_posix.h"
#include "fm02_posix.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <limits.h>
int main(void){
    char tmp[]="/tmp/spmp-fm04-XXXXXX",before[PATH_MAX],after[PATH_MAX];
    fm01_posix fs1;fm02_posix fs2;
    fm01_model browser;fm02_model action;fm04_model k;
    FILE *f;
    assert(mkdtemp(tmp));
    {char src[PATH_MAX];snprintf(src,sizeof(src),"%s/FROM",tmp);assert(mkdir(src,0700)==0);}
    snprintf(before,sizeof(before),"%s/FROM/Журнал 11.11.26.log",tmp);
    snprintf(after,sizeof(after),"%s/FROM/Новое имя.txt",tmp);
    f=fopen(before,"wb");assert(f);assert(fwrite("test\n",1,5,f)==5);fclose(f);
    fs1.rootfd=-1;fs2.rootfd=-1;
    assert(fm01_posix_init(&fs1,tmp)==0);
    assert(fm02_posix_init(&fs2,tmp)==0);
    fm01_init(&browser,fm01_posix_list,&fs1);
    browser.sd_present=1;
    strcpy(browser.panels[0].path,"FROM");
    assert(fm01_refresh(&browser,0)==0);
    assert(fm01_selected(&browser,0));
    fm02_init(&action,&browser,fm02_posix_action,&fs2);
    action.state=FM02_RENAME_INPUT_REQUIRED;
    assert(fm04_open_rename(&k,&action)==0);
    assert(strcmp(k.value,"Журнал 11.11.26.log")==0);
    assert(fm04_activate(&k,41)==FM04_CHANGED); /* CLEAR */
    assert(k.value[0]==0);
    {
        const char *chars[]={"Н","о","в","о","е"," ","и","м","я",".","t","x","t"};
        unsigned i;for(i=0;i<sizeof(chars)/sizeof(chars[0]);i++)assert(fm04_insert(&k,chars[i])==0);
    }
    assert(fm04_finish(&k)==FM04_ACCEPT);
    assert(access(before,F_OK)!=0 && access(after,F_OK)==0);
    assert(action.state==FM02_DONE);
    f=fopen(after,"rb");assert(f);{char test[6]={0};assert(fread(test,1,5,f)==5);assert(strcmp(test,"test\n")==0);}fclose(f);
    fm01_posix_close(&fs1);fm02_posix_close(&fs2);
    assert(unlink(after)==0);
    {char src[PATH_MAX];snprintf(src,sizeof(src),"%s/FROM",tmp);assert(rmdir(src)==0);}
    assert(rmdir(tmp)==0);
    puts("FM04: real FM01+FM02 POSIX rename of UTF-8 file: OK");return 0;
}
