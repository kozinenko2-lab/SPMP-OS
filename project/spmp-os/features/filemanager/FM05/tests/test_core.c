/* SPDX-License-Identifier: MIT */
#include "spmp_fm05.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
typedef struct {char data[FM05_TEXT_CAP+1];unsigned n;uint64_t rev;int fail_save;} mock;
static int load(void *p,const char *path,char *dst,unsigned cap,unsigned *n,uint64_t *rev){
    mock*m=p;(void)path;if(m->n>cap)return FM05_ERR_TOO_LARGE;memcpy(dst,m->data,m->n);*n=m->n;*rev=m->rev;return 0;
}
static int save(void *p,const char *path,const char *src,unsigned n,uint64_t expected,uint64_t *updated){
    mock*m=p;(void)path;if(m->fail_save)return -5;if(expected!=m->rev)return FM05_ERR_CONFLICT;
    memcpy(m->data,src,n);m->n=n;m->data[n]=0;*updated=++m->rev;return 0;
}
static void test_edit(void){
    mock fs={.data="Первый\r\nВторой\r\nТретий\r\n",.rev=12};fm05_doc d;fm04_model kb;
    fs.n=(unsigned)strlen(fs.data);
    fm05_init(&d,(fm05_fs){load,save,&fs});
    assert(fm05_open(&d,"notes/Журнал.log",0)==0);
    assert(d.lines==4&&d.crlf==1&&d.length==fs.n);
    assert(fm05_newline(&d)==0&&d.text[0]=='\r'&&d.text[1]=='\n');
    assert(fm05_undo(&d)==0&&d.length==fs.n&&d.text[0]!='\r');
    assert(fm05_insert(&d,"Я",2)==0);
    assert(d.caret==2&&d.text[0]==(char)0xd0);
    fm05_left(&d);assert(d.caret==0);
    fm05_right(&d);assert(d.caret==2);
    assert(fm05_backspace(&d)==0&&d.caret==0);
    assert(fm05_delete(&d)==0); /* deletes first UTF8 character */
    assert(fm05_undo(&d)==0);
    assert(fm05_find(&d,"Третий",1)==1);
    assert(fm05_caret_line(&d)==2);
    fm05_line_end(&d);assert(fm05_caret_column(&d)==6);
    fm05_up(&d);assert(fm05_caret_line(&d)==1);
    fm05_down(&d);assert(fm05_caret_line(&d)==2);
    fm05_page(&d,-1);assert(fm05_caret_line(&d)==0);
    assert(fm05_open_keyboard(&d,&kb,FM05_INPUT_TEXT)==0);
    assert(fm04_insert(&kb,"Й")==0);
    assert(fm04_finish(&kb)==FM04_ACCEPT);
    assert(fm05_finish_keyboard(&d,&kb,FM05_INPUT_TEXT)==0);
    assert(d.dirty);
    fs.fail_save=1;assert(fm05_save(&d)==-5&&d.dirty);
    fs.fail_save=0;assert(fm05_save(&d)==0&&!d.dirty&&fs.rev==13);
    assert(fm05_request_close(&d)==1);
}
static void test_guard(void){
    mock fs={.data="x\n",.n=2,.rev=7};fm05_doc d;fm04_model kb;
    fm05_init(&d,(fm05_fs){load,save,&fs});
    assert(fm05_open(&d,"test.abc",1)==FM05_ERR_PATH);
    assert(fm05_open(&d,"ok.TXT",1)==0);
    assert(fm05_find(&d,"xyz",1)==0);
    assert(fm05_find(&d,"x",1)==1);
    assert(fm05_insert(&d,"\xff",1)==FM05_ERR_ENCODING);
    assert(fm05_insert(&d,"\xC0\x80",2)==FM05_ERR_ENCODING);
    assert(fm05_insert(&d,"\n",1)==FM05_ERR_ENCODING);
    assert(fm05_insert(&d,"é",2)==0);
    fs.rev++;assert(fm05_save(&d)==FM05_ERR_CONFLICT&&d.dirty);
    assert(fm05_request_close(&d)==0&&d.state==FM05_DISCARD_CONFIRM);
    fm05_confirm_close(&d,0);assert(d.state==FM05_OPEN);
    assert(fm05_open(&d,"new.log",0)==FM05_ERR_CONFLICT);
    fm05_confirm_close(&d,1); /* does not discard unless confirmation state */
    assert(fm05_request_close(&d)==0);fm05_confirm_close(&d,1);
    assert(d.state==FM05_CLOSED);
    assert(fm05_open(&d,"new.ini",0)==0);
    assert(fm05_open_keyboard(&d,&kb,FM05_INPUT_SEARCH)==0);
    assert(fm04_insert(&kb,"x")==0);
    assert(fm04_finish(&kb)==FM04_ACCEPT);
    assert(fm05_finish_keyboard(&d,&kb,FM05_INPUT_SEARCH)==1);
    assert(fm05_utf8_valid("abc",3));
    assert(!fm05_utf8_valid("\xED\xA0\x80",3));
    assert(!fm05_utf8_valid("ab\0cd",5));
    assert(fm05_extension_supported("a.CFG")&&fm05_extension_supported("foo.XML"));
    assert(!fm05_extension_supported("x.jpg")&&!fm05_extension_supported(".no"));
}
static void test_limit(void){
    mock fs={.n=FM05_TEXT_CAP,.rev=5};fm05_doc d;unsigned i;
    for(i=0;i<fs.n;i++)fs.data[i]='a';
    fm05_init(&d,(fm05_fs){load,save,&fs});
    assert(fm05_open(&d,"big.txt",1)==0);
    assert(fm05_insert(&d,"b",1)==FM05_ERR_TOO_LARGE);
    assert(d.length==FM05_TEXT_CAP&&!d.dirty);
    fs.n=FM05_TEXT_CAP+1u;
    assert(fm05_open(&d,"big.txt",1)==FM05_ERR_TOO_LARGE);
}
static void test_random(void){
    mock fs={.n=0,.rev=99};fm05_doc d;uint32_t seed=0x31415926u;
    unsigned i;
    fm05_init(&d,(fm05_fs){load,save,&fs});
    assert(fm05_open(&d,"random.log",0)==0);
    for(i=0;i<40000u;i++){
        seed=1664525u*seed+1013904223u;
        switch((seed>>24u)%8u){
            case 0:(void)fm05_insert(&d,"я",2);break;
            case 1:(void)fm05_insert(&d,"a",1);break;
            case 2:(void)fm05_newline(&d);break;
            case 3:(void)fm05_backspace(&d);break;
            case 4:(void)fm05_delete(&d);break;
            case 5:fm05_left(&d);break;
            case 6:fm05_right(&d);break;
            default:fm05_up(&d);break;
        }
        assert(d.caret<=d.length&&d.length<=FM05_TEXT_CAP);
        assert(fm05_utf8_valid(d.text,d.length));
        assert(d.text[d.length]==0);
        if(i%317u==0u){unsigned c=fm05_caret_line(&d);assert(c<d.lines);}
    }
    assert(fm05_save(&d)==0);
}

static void test_session(void){
    mock fs={.data="line 1\nline 2\n",.n=14,.rev=15};
    fm05_session s;
    fm05_session_init(&s,(fm05_fs){load,save,&fs});
    assert(fm05_session_open(&s,"system.ini",0)==0);
    assert(fm05_session_buttons(&s,FM01_KEY_A)==0);
    assert(s.input_mode==FM05_INPUT_TEXT);
    assert(fm04_insert(&s.keyboard,"Ж")==0);
    assert(fm05_session_buttons(&s,FM01_KEY_START)==0);
    assert(s.doc.dirty&&s.input_mode==FM05_INPUT_NONE);
    assert(fm05_session_buttons(&s,FM01_KEY_START|FM01_KEY_A)==0);
    assert(fm05_session_buttons(&s,FM01_KEY_START|FM01_KEY_SELECT)==0);
    assert(!s.doc.dirty);
    assert(fm05_session_buttons(&s,FM01_KEY_SELECT)==0);
    assert(s.input_mode==FM05_INPUT_SEARCH);
    assert(fm04_insert(&s.keyboard,"l")==0);
    assert(fm04_insert(&s.keyboard,"i")==0);
    assert(fm04_insert(&s.keyboard,"n")==0);
    assert(fm04_insert(&s.keyboard,"e")==0);
    assert(fm05_session_buttons(&s,FM01_KEY_START)>=0);
    assert(s.input_mode==FM05_INPUT_NONE);
    assert(fm05_session_buttons(&s,FM01_KEY_B)==1);
    assert(s.doc.state==FM05_CLOSED);
    printf("FM05 session RAM: %zu bytes, doc: %zu bytes\n",sizeof(s),sizeof(s.doc));
}
int main(void){test_edit();test_guard();test_limit();test_random();test_session();puts("FM05 core: edit/UTF8/CRLF/keyboard/search/undo/safe save/40000 fuzz OK");return 0;}
