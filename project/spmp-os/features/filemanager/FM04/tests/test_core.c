#include "spmp_fm04.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static uint32_t seed=0x12a3db24u;
static unsigned rnd(void){seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;return seed;}
static int is_boundary(const fm04_model *k){return k->cursor<=k->bytes&&(k->cursor==k->bytes||((unsigned char)k->value[k->cursor]&0xc0u)!=0x80u);}
static int calls;
static const char *german(void *unused,unsigned token){(void)unused;return token==FM04_TEXT_RENAME?"Umbenennen":0;}
static int dummy_backend(void *ctx,const fm02_request *req,fm02_progress_hook hook,void *hctx,fm02_progress *progress){
    (void)ctx;(void)hook;(void)hctx;(void)progress;
    assert(req->action==FM02_RENAME);assert(strcmp(req->name,"старое.txt")==0);
    assert(strcmp(req->new_name,"Новое имя.txt")==0);
    calls++;return 0;
}
static int listing(void *ctx,const char *path,uint32_t off,uint32_t cap,fm01_entry *items,
    uint32_t *got,uint32_t *total,uint32_t *files,uint32_t *dirs){
    (void)ctx;(void)path;
    *total=1;*files=1;*dirs=0;
    *got=off?0u:(cap?1u:0u);
    if(*got){memset(items,0,sizeof(*items));strcpy(items->name,"старое.txt");items->type=FM01_ENTRY_FILE;}
    return 0;
}
int main(void){
    fm04_model k;fm01_model browser;fm02_model f;unsigned i;char buf[FM01_NAME_MAX];
    assert(fm04_open(&k,"Привет 123.txt",FM04_FILENAME,0)==0);
    assert(k.cursor==strlen("Привет 123.txt"));
    fm04_move_cursor(&k,-1);assert(k.value[k.cursor]=='t');
    assert(fm04_delete(&k)==1);assert(strcmp(k.value,"Привет 123.tx")==0);
    assert(fm04_insert(&k,"t")==0);assert(strcmp(k.value,"Привет 123.txt")==0);
    assert(fm04_utf8_valid(k.value,FM01_NAME_MAX));
    assert(fm04_open(&k,"ёЖ👍",FM04_LINE,0)==0);
    assert(fm04_backspace(&k)==1);assert(strcmp(k.value,"ёЖ")==0);
    assert(fm04_delete(&k)==0);
    fm04_move_cursor(&k,-1);assert(fm04_delete(&k)==1);assert(strcmp(k.value,"ё")==0);
    assert(fm04_insert(&k,"🙂")==0);assert(strcmp(k.value,"ё🙂")==0);
    assert(!fm04_utf8_valid("\xc0\xaf",FM01_NAME_MAX));
    assert(!fm04_utf8_valid("\xed\xa0\x80",FM01_NAME_MAX));
    assert(!fm04_utf8_valid("\xf4\x90\x80\x80",FM01_NAME_MAX));
    assert(!fm04_utf8_valid("\xe2\x82",FM01_NAME_MAX));
    assert(!fm04_filename_valid("."));assert(!fm04_filename_valid(".."));
    assert(!fm04_filename_valid("FOLDER/FILE"));assert(!fm04_filename_valid("CON.txt"));
    assert(!fm04_filename_valid("test."));assert(!fm04_filename_valid("trailing "));
    assert(fm04_filename_valid("тестовый-файл 2026.txt"));
    assert(fm04_open(&k,"",FM04_FILENAME,0)==0);
    assert(fm04_finish(&k)==FM04_ERROR && k.state==FM04_EDITING);
    assert(fm04_activate(&k,11)==FM04_CHANGED);
    assert(k.bytes>0);assert(k.value[0]!=0);
    assert(fm04_activate(&k,33)==FM04_CHANGED && k.shift==1);
    assert(fm04_activate(&k,34)==FM04_CHANGED && k.layout==FM04_EN);
    assert(fm04_activate(&k,35)==FM04_CHANGED && k.layout==FM04_SYMBOLS);
    assert(fm04_activate(&k,35)==FM04_CHANGED && k.layout==FM04_RU);
    assert(fm04_activate(&k,43)==FM04_ACCEPT);
    assert(fm04_open(&k,"abc",FM04_FILENAME,1)==0);
    fm04_set_translation(&k,german,NULL);
    assert(strcmp(fm04_localize(&k,FM04_TEXT_RENAME),"Umbenennen")==0);
    assert(strcmp(fm04_localize(&k,FM04_TEXT_OK),"OK")==0);
    assert(fm04_buttons(&k,FM01_KEY_B|FM01_KEY_SELECT)==FM04_CANCEL && k.state==FM04_CANCELLED);
    assert(fm04_open(&k,"abcdefghij",FM04_FILENAME,1)==0);
    for(i=0;i<120000u;i++){
        unsigned op=rnd()%10u;
        if(op==0u)fm04_move_cursor(&k,-1);
        else if(op==1u)fm04_move_cursor(&k,1);
        else if(op==2u)fm04_backspace(&k);
        else if(op==3u)fm04_delete(&k);
        else if(op==4u)fm04_insert(&k,"a");
        else if(op==5u)fm04_insert(&k,"ё");
        else if(op==6u)fm04_insert(&k,"🎵");
        else if(op==7u)fm04_insert(&k," ");
        else if(op==8u)fm04_insert(&k,"/");
        else fm04_insert(&k,"_");
        assert(is_boundary(&k));assert(fm04_utf8_valid(k.value,FM01_NAME_MAX));
        assert(strlen(k.value)==k.bytes);
    }
    memset(buf,'a',255);buf[255]=0;
    assert(fm04_open(&k,buf,FM04_FILENAME,0)==0);
    assert(fm04_insert(&k,"z")==-4);
    fm01_init(&browser,listing,NULL);browser.sd_present=1;
    assert(fm01_refresh(&browser,0)==0);
    fm02_init(&f,&browser,dummy_backend,NULL);f.state=FM02_RENAME_INPUT_REQUIRED;
    assert(fm04_open_rename(&k,&f)==0);
    assert(strcmp(k.value,"старое.txt")==0);
    assert(fm04_open(&k,"Новое имя.txt",FM04_FILENAME,0)==0);
    k.rename_target=&f;
    assert(fm04_finish(&k)==FM04_ACCEPT && calls==1 && f.state==FM02_DONE);
    assert(fm04_open(&k,"",FM04_FILENAME,0)==0);
    assert(fm04_insert(&k,"/")==-3);
    puts("FM04: UTF-8 / Russian+English+symbols / filename guard / editing / FM02 rename / 120000 randomized operations: OK");
    return 0;
}
