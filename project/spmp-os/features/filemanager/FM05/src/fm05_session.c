/* SPDX-License-Identifier: MIT
 * FM-05 UI controller: key combinations, reusable FM-04 keyboard, confirmation.
 * No OS-specific dependencies.
 */
#include "spmp_fm05.h"
#include <stdint.h>
void fm05_session_init(fm05_session *s,fm05_fs fs){if(!s)return;fm05_init(&s->doc,fs);s->input_mode=FM05_INPUT_NONE;}
int fm05_session_open(fm05_session *s,const char *path,unsigned locale){if(!s)return -1;s->input_mode=FM05_INPUT_NONE;return fm05_open(&s->doc,path,locale);}
int fm05_session_buttons(fm05_session *s,uint32_t keys){
    fm05_doc *d;fm04_event ev;int rc;
    if(!s)return -1;d=&s->doc;
    if(s->input_mode!=FM05_INPUT_NONE){
        ev=fm04_buttons(&s->keyboard,keys);
        if(ev==FM04_ACCEPT){
            rc=fm05_finish_keyboard(d,&s->keyboard,(fm05_input_mode)s->input_mode);
            s->input_mode=FM05_INPUT_NONE;return rc;
        }
        if(ev==FM04_CANCEL){s->input_mode=FM05_INPUT_NONE;return 0;}
        return ev==FM04_ERROR?-2:0;
    }
    if(d->state==FM05_DISCARD_CONFIRM){
        if(keys&FM01_KEY_A){fm05_confirm_close(d,1);return 1;}
        if(keys&FM01_KEY_B){fm05_confirm_close(d,0);return 0;}
        return 0;
    }
    if(d->state!=FM05_OPEN)return -1;
    if((keys&(FM01_KEY_SELECT|FM01_KEY_START))==(FM01_KEY_SELECT|FM01_KEY_START))return fm05_save(d);
    if((keys&(FM01_KEY_START|FM01_KEY_A))==(FM01_KEY_START|FM01_KEY_A))return fm05_newline(d);
    if((keys&(FM01_KEY_START|FM01_KEY_UP))==(FM01_KEY_START|FM01_KEY_UP)){fm05_page(d,-1);return 0;}
    if((keys&(FM01_KEY_START|FM01_KEY_DOWN))==(FM01_KEY_START|FM01_KEY_DOWN)){fm05_page(d,1);return 0;}
    if((keys&(FM01_KEY_START|FM01_KEY_LEFT))==(FM01_KEY_START|FM01_KEY_LEFT)){fm05_line_start(d);return 0;}
    if((keys&(FM01_KEY_START|FM01_KEY_RIGHT))==(FM01_KEY_START|FM01_KEY_RIGHT)){fm05_line_end(d);return 0;}
    if(keys&FM01_KEY_A){
        rc=fm05_open_keyboard(d,&s->keyboard,FM05_INPUT_TEXT);
        if(!rc)s->input_mode=FM05_INPUT_TEXT;
        return rc;
    }
    if(keys&FM01_KEY_SELECT){
        rc=fm05_open_keyboard(d,&s->keyboard,FM05_INPUT_SEARCH);
        if(!rc)s->input_mode=FM05_INPUT_SEARCH;
        return rc;
    }
    if(keys&FM01_KEY_B)return fm05_request_close(d);
    fm05_buttons(d,keys);return 0;
}
void fm05_session_render(const fm05_session *s,uint16_t *fb,unsigned pitch,
    const fm01_palette *colors,const fm01_font *font){
    if(!s)return;
    fm05_render(&s->doc,fb,pitch,colors,font);
    if(s->input_mode!=FM05_INPUT_NONE)fm04_render_overlay(&s->keyboard,fb,pitch,colors,font);
}
