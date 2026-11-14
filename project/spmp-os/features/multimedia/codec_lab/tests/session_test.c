#include "spmp_ceva_session.h"
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"FAIL %s:%d: %s\n",__FILE__,__LINE__,#x);return 1;}}while(0)
static int state_tests(void){
 spmp_ceva_session s;spmp_ceva_session_init(&s,4096);
 CHECK(s.state==SPMP_CEVA_OFF&&spmp_ceva_session_event(&s,SPMP_CEVA_SIGNAL_READY)!=0);
 CHECK(spmp_ceva_session_event(&s,SPMP_CEVA_LOAD_OS_EXT)==0);
 CHECK(spmp_ceva_session_event(&s,SPMP_CEVA_SIGNAL_READY)==0);
 CHECK(spmp_ceva_session_event(&s,SPMP_CEVA_LOAD_OS_INT)==0);
 CHECK(spmp_ceva_session_event(&s,SPMP_CEVA_SIGNAL_CODEC_BOOTED)==0);
 CHECK(spmp_ceva_session_event(&s,SPMP_CEVA_BEGIN_STOP)==0);
 CHECK(spmp_ceva_session_event(&s,SPMP_CEVA_COMPLETE_STOP)==0&&s.state==SPMP_CEVA_OFF);
 CHECK(spmp_ceva_session_event(&s,SPMP_CEVA_FAIL)!=0);return 0;
}
static int packet_tests(void){
 uint8_t packet[257]={0},opcode,no_reply;uint16_t payload;
 packet[0]=0x41;packet[1]=2;packet[2]=252;
 CHECK(spmp_ceva_request_validate(packet,256,&opcode,&no_reply,&payload)==0);
 CHECK(opcode==0x41&&no_reply==1&&payload==252);
 packet[2]=253;CHECK(spmp_ceva_request_validate(packet,257,0,0,0)!=0);
 packet[2]=4;packet[0]=0x3f;CHECK(spmp_ceva_request_validate(packet,8,0,0,0)!=0);
 packet[0]=0x43;CHECK(spmp_ceva_request_validate(packet,7,0,0,0)!=0);return 0;
}
static int allocation_tests(void){
 spmp_ceva_session s;uint32_t released=0,i;spmp_ceva_session_init(&s,128);
 CHECK(spmp_ceva_track_allocation(&s,0x1000,16)!=0);
 CHECK(spmp_ceva_session_event(&s,SPMP_CEVA_LOAD_OS_EXT)==0);
 CHECK(spmp_ceva_track_allocation(&s,0x1000,16)!=0);
 CHECK(spmp_ceva_session_event(&s,SPMP_CEVA_SIGNAL_READY)==0);
 CHECK(spmp_ceva_track_allocation(&s,0x1000,64)==0);
 CHECK(spmp_ceva_track_allocation(&s,0x1000,16)!=0);
 CHECK(spmp_ceva_track_allocation(&s,0x1002,16)!=0);
 CHECK(spmp_ceva_track_allocation(&s,0x2000,65)!=0);
 CHECK(spmp_ceva_session_event(&s,SPMP_CEVA_BEGIN_STOP)==0);
 CHECK(spmp_ceva_session_event(&s,SPMP_CEVA_COMPLETE_STOP)!=0);
 CHECK(spmp_ceva_track_free(&s,0x2000,&released)!=0);
 CHECK(spmp_ceva_track_free(&s,0x1000,&released)==0&&released==64);
 CHECK(spmp_ceva_session_event(&s,SPMP_CEVA_COMPLETE_STOP)==0);
 spmp_ceva_session_init(&s,1024);CHECK(spmp_ceva_session_event(&s,SPMP_CEVA_LOAD_OS_EXT)==0);
 CHECK(spmp_ceva_session_event(&s,SPMP_CEVA_SIGNAL_READY)==0);
 for(i=0;i<SPMP_CEVA_MAX_ALLOCATIONS;i++)CHECK(spmp_ceva_track_allocation(&s,0x4000u+i*4u,4)==0);
 CHECK(spmp_ceva_track_allocation(&s,0x5000,4)!=0);return 0;
}
int main(void){CHECK(state_tests()==0);CHECK(packet_tests()==0);CHECK(allocation_tests()==0);
 puts("CEVA session state/bounds/ownership tests passed");return 0;}
