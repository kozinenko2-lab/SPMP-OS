#include "spmp_theme_config_v8.h"
#include "spmp_theme_catalog_v8.h"
#include <stddef.h>
static uint32_t crc(const uint8_t*p,size_t n){uint32_t c=0xffffffffu;
 for(size_t i=0;i<n;i++){c^=p[i];for(unsigned j=0;j<8;j++)c=(c>>1)^(0xedb88320u& (0u-(c&1u)));}return ~c;}
int spmp_theme_v8_config_encode(const char *id,uint8_t layout,uint8_t out[40]){
 if(!out||layout>1||spmp_theme_v8_find(id)<0)return -1;
 for(unsigned i=0;i<40;i++)out[i]=0;
 out[0]='S';out[1]='P';out[2]='T';out[3]='C';out[4]=1;out[5]=layout;
 unsigned i=0;while(id[i]){if(i>=27)return -2;out[8+i]=(uint8_t)id[i];i++;}
 uint32_t c=crc(out,36);for(unsigned j=0;j<4;j++)out[36+j]=(uint8_t)(c>>(8*j));return 0;}
int spmp_theme_v8_config_decode(const uint8_t in[40],unsigned*index,uint8_t*layout){
 if(!in||!index||!layout||in[0]!='S'||in[1]!='P'||in[2]!='T'||in[3]!='C'||in[4]!=1||in[5]>1||in[6]||in[7])return -1;
 uint32_t sum=0;for(unsigned j=0;j<4;j++)sum|=(uint32_t)in[36+j]<<(8*j);
 if(crc(in,36)!=sum||in[35])return -2;
 char id[28];unsigned i=0;for(;i<28;i++){id[i]=(char)in[8+i];if(!id[i])break;}
 if(i==0||i==28)return -3;
 int n=spmp_theme_v8_find(id);if(n<0)return -4;
 *index=(unsigned)n;*layout=in[5];return 0;
}
unsigned spmp_theme_v8_cycle(unsigned n,int delta){unsigned size=(unsigned)spmp_theme_v8_count();
 if(!size)return 0;unsigned initial=n<size?n:0;
 if(delta>=0)return (initial+(unsigned)delta%size)%size;
 /* Convert negative delta without integer-min negation UB. */
 unsigned mag=(unsigned)(-(delta+1))+1u;
 return (initial+size-(mag%size))%size;
}
