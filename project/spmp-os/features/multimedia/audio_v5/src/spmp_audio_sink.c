#include "spmp_audio_v5.h"
int sp_audio_pump(sp_audio_player*p,sp_audio_sink sink,int16_t*scratch,size_t cap,size_t*done){
 sp_audio_info info;int rc;size_t n=0;
 if(done)*done=0;
 if(!p||!scratch||!cap||!done||!sink.write)return SP_AUD_BAD;
 info=sp_audio_get_info(p);if(info.channels<1||info.channels>2)return SP_AUD_UNSUPPORTED;
 rc=sp_audio_decode(p,scratch,cap,&n);if(rc)return rc;
 if(n && sink.write(sink.ctx,scratch,n))return SP_AUD_IO;
 *done=n;return 0;
}
