/* Two-pane read-only host console: confirm real data and navigation in FM-01. */
#include "fm01_posix.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
static void print_panel(const fm01_model *m,unsigned which) {
    const fm01_panel *p=&m->panels[which];
    unsigned i;char date[16],size[32];
    printf("[%c] /%s | files=%u dirs=%u total=%u selection=%u\n",
        m->active==which?'*':' ',p->path,p->file_count,p->dir_count,p->total,p->cursor);
    for(i=0;i<p->loaded;i++) {
        const fm01_entry *e=&p->entries[i];
        fm01_date_text(e,date,sizeof(date));fm01_size_text(e->size_bytes,size,sizeof(size));
        printf(" %c %s %-42.42s %10s %s\n",
            p->cursor==p->first+i?'>':' ',e->type==FM01_ENTRY_DIR?"DIR ":"FILE",
            e->name,e->type==FM01_ENTRY_DIR?"--":size,date);
    }
}
int main(int argc,char **argv) {
    fm01_posix fs={-1};fm01_model *m;const char *root=NULL,*left=NULL,*right=NULL;
    char buf[128];int i;
    for(i=1;i<argc;i++) {
        if(!strcmp(argv[i],"--root") && i+1<argc)root=argv[++i];
        else if(!strcmp(argv[i],"--left") && i+1<argc)left=argv[++i];
        else if(!strcmp(argv[i],"--right") && i+1<argc)right=argv[++i];
        else {fprintf(stderr,"usage: %s --root /path [--left folder] [--right folder]\n",argv[0]);return 2;}
    }
    if(!root || fm01_posix_init(&fs,root)) {fprintf(stderr,"Cannot open SD root\n");return 2;}
    m=(fm01_model*)calloc(1,sizeof(*m));if(!m){fm01_posix_close(&fs);return 2;}
    fm01_init(m,fm01_posix_list,&fs);
    if((left && strlen(left)>=FM01_PATH_MAX) || (right && strlen(right)>=FM01_PATH_MAX))return 2;
    if(left){strcpy(m->panels[0].path,left);fm01_refresh(m,0);}
    if(right){strcpy(m->panels[1].path,right);fm01_refresh(m,1);}
    puts("FM-01 read-only. Keys: j/k=down/up, h/l=left/right, o=enter, b=parent, t=toggle, r=refresh, q=quit");
    print_panel(m,0);print_panel(m,1);
    while(fgets(buf,sizeof(buf),stdin)) {
        switch(buf[0]){
            case 'j':fm01_buttons(m,FM01_KEY_DOWN);break;
            case 'k':fm01_buttons(m,FM01_KEY_UP);break;
            case 'h':fm01_buttons(m,FM01_KEY_LEFT);break;
            case 'l':fm01_buttons(m,FM01_KEY_RIGHT);break;
            case 'o':fm01_buttons(m,FM01_KEY_A);break;
            case 'b':fm01_buttons(m,FM01_KEY_B);break;
            case 't':fm01_buttons(m,FM01_KEY_SELECT);break;
            case 'r':fm01_refresh(m,m->active);break;
            case 'q':free(m);fm01_posix_close(&fs);return 0;
            default:puts("Unknown key");continue;
        }
        print_panel(m,0);print_panel(m,1);
    }
    free(m);fm01_posix_close(&fs);return 0;
}
