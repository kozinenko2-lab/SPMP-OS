#include "spmp_fm02.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
static int fs_list(void *ctx,const char *path,uint32_t offset,uint32_t cap,
                   fm01_entry *out,uint32_t *got,uint32_t *total,uint32_t *files,uint32_t *dirs){
    const char *names[]={"Бета.txt","Заметки.log","game.zip"};unsigned j;
    (void)ctx;(void)path;
    *total=3;*files=3;*dirs=0;*got=cap?3-offset:0;
    if(*got>cap)*got=cap;
    for(j=0;j<*got;j++){
        memset(&out[j],0,sizeof(*out));strcpy(out[j].name,names[j+offset]);
        out[j].type=FM01_ENTRY_FILE;out[j].size_bytes=100;
    }
    return 0;
}
static fm02_request last;
static unsigned calls=0,abort_after=0;
static int fake(void *ctx,const fm02_request *req,fm02_progress_hook hook,void *hookctx,fm02_progress *out){
    (void)ctx;last=*req;calls++;
    out->phase=2;out->bytes_done=512;out->bytes_total=1024;out->items_done=0;
    if((abort_after && calls>=abort_after && hook(hookctx,out)) || (!abort_after && hook(hookctx,out)))return -125;
    out->bytes_done=1024;out->items_done=1;
    return 0;
}
static int always_cancel(void *unused){(void)unused;return 1;}
int main(void){
    fm01_model browse;fm02_model fm;
    fm01_init(&browse,fs_list,0);fm02_init(&fm,&browse,fake,0);
    assert(fm02_valid_name("Привет.txt") && fm02_valid_name("ééé.dat"));
    assert(!fm02_valid_name("../x")&&!fm02_valid_name("x/y")&&!fm02_valid_name("..") && !fm02_valid_name(""));
    assert(!fm02_valid_name("\xc0\xaf")&&!fm02_valid_name("\xed\xa0\x80")&&!fm02_valid_name("a\\b"));
    assert(fm02_toggle_mark(&fm)==1 && fm.marks_count==1);
    assert(fm02_is_marked(&fm,"","Бета.txt",0));
    fm02_buttons(&fm,FM01_KEY_DOWN);
    assert(fm02_toggle_mark(&fm)==1 && fm.marks_count==2);
    assert(fm02_prepare(&fm,FM02_COPY,0)==-5);/* both panes same dir */
    strcpy(browse.panels[1].path,"DEST");
    assert(fm02_prepare(&fm,FM02_COPY,0)==0);
    calls=0;assert(fm02_commit(&fm,1)==0 && calls==2 && fm.processed==2);
    assert(fm.marks_count==0 && fm.state==FM02_DONE);
    assert(!strcmp(last.name,"Заметки.log") && !strcmp(last.dst_directory,"DEST"));
    /* Cancel delete keeps all files and never calls backend. */
    assert(fm02_prepare(&fm,FM02_DELETE,0)==0 && fm.state==FM02_CONFIRM_DELETE);
    calls=0;assert(fm02_commit(&fm,0)==1 && calls==0 && fm.state==FM02_CANCELLED);
    assert(fm02_prepare(&fm,FM02_RENAME,"Новый файл.txt")==0);
    assert(fm02_commit(&fm,1)==0 && !strcmp(last.new_name,"Новый файл.txt"));
    assert(fm02_prepare(&fm,FM02_RENAME,"../bad")==-4);
    /* Start opens action menu, cancel by B, select toggles mark. */
    fm02_buttons(&fm,FM01_KEY_START);assert(fm.state==FM02_MENU);
    fm02_buttons(&fm,FM01_KEY_B);assert(fm.state==FM02_IDLE);
    fm02_buttons(&fm,FM01_KEY_SELECT);assert(fm.marks_count==1);
    fm02_clear_marks(&fm);
    fm02_buttons(&fm,FM01_KEY_START);
    fm02_buttons(&fm,FM01_KEY_DOWN);fm02_buttons(&fm,FM01_KEY_DOWN);
    fm02_buttons(&fm,FM01_KEY_A);assert(fm.state==FM02_RENAME_INPUT_REQUIRED);
    assert(fm02_supply_rename(&fm,"Документ.txt")==0 && fm.state==FM02_DONE);
    strcpy(browse.panels[1].path,"DEST");
    fm02_set_cancel_poll(&fm,always_cancel,0);
    assert(fm02_prepare(&fm,FM02_COPY,0)==0);
    assert(fm02_commit(&fm,1)==-125 && fm.state==FM02_CANCELLED);
    fm02_set_cancel_poll(&fm,0,0);
    /* Active panel cursor preserved after failed batch and marks retained */
    printf("FM02 core tests passed; calls=%u, model bytes=%zu\n",calls,sizeof(fm));
    return 0;
}
