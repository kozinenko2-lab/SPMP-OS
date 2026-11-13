#include "spmp_stage48_sd_monitor.h"
#define SPMP48_MAGIC 0x38345053u /* 'SP48' */
static uint32_t sat_inc(uint32_t x){return x==UINT32_MAX?x:x+1u;}
void spmp48_monitor_init(spmp48_monitor *m,spmp_bootlog *log,uint32_t detailed){
 uint32_t i;
 if(!m)return;
 m->magic=SPMP48_MAGIC;m->head=m->count=m->dropped=m->total=0;
 m->next_sequence=1;m->successes=m->failures=m->cmd55_pending=0;
 m->last_command=UINT32_MAX;
 for(i=0;i<SPMP48_EVENT_CAP;i++){
   spmp48_event *e=&m->entries[i];e->sequence=e->tick=e->command_word=0;
   e->argument=e->response0=e->controller_status=e->flags=0;
   e->result=e->classification=0;
 }
 m->optional_bootlog=log;m->detailed=(detailed!=0);
}
int spmp48_monitor_observe(spmp48_monitor *m,uint32_t tick,
 uint32_t word,uint32_t argument,uint32_t response,uint32_t status,
 uint32_t flags,enum spmp48_outcome result){
 uint32_t cmd,klass;
 spmp48_event *e;
 if(!m || m->magic!=SPMP48_MAGIC || (flags&~SPMP48_ALLOWED_FLAGS) ||
    (uint32_t)result>(uint32_t)SPMP48_NOT_SUPPORTED || m->head>=SPMP48_EVENT_CAP ||
    m->count>SPMP48_EVENT_CAP || m->next_sequence==0) return 0;
 cmd=word&63u;
 klass=SPMP48_NORMAL;
 if(cmd==6u || cmd==41u)
   klass=m->cmd55_pending?SPMP48_APP:SPMP48_UNCLASSIFIED;
 /* A successful CMD55 authorizes classification of the very next event.
  * Failures and unrelated commands clear the marker. */
 m->cmd55_pending=(cmd==55u && result==SPMP48_SUCCESS)?1u:0u;
 m->total=sat_inc(m->total);
 if(result==SPMP48_SUCCESS)m->successes=sat_inc(m->successes);
 else m->failures=sat_inc(m->failures);
 m->last_command=cmd;
 e=&m->entries[m->head];
 e->sequence=m->next_sequence;
 m->next_sequence++;if(!m->next_sequence)m->next_sequence=1u;
 e->tick=tick;e->command_word=word;e->argument=argument;
 e->response0=(flags&SPMP48_FLAG_RESPONSE_OBSERVED)?response:0;
 e->controller_status=(flags&SPMP48_FLAG_CONTROLLER_STATUS_OBSERVED)?status:0;
 e->flags=flags;e->result=(uint32_t)result;e->classification=klass;
 m->head=(m->head+1u)%SPMP48_EVENT_CAP;
 if(m->count==SPMP48_EVENT_CAP)m->dropped=sat_inc(m->dropped);
 else m->count++;
 if(m->optional_bootlog && (m->detailed || result!=SPMP48_SUCCESS)){
   /* RAM-only append; never sync or write SD from here. Store raw word and
    * argument; status and response remain in the monitor's own ring. */
   spmp_log_event(m->optional_bootlog,tick,SPMP_LOG_SD_SCAN,
     result==SPMP48_SUCCESS?SPMP_LOG_DEBUG:SPMP_LOG_ERROR,
     SPMP48_EVENT_BASE+cmd,word,argument,
     result==SPMP48_SUCCESS?"SD command observed":"SD command failed");
 }
 return 1;
}
int spmp48_monitor_get(const spmp48_monitor *m,uint32_t index,spmp48_event*out){
 uint32_t first;
 if(!m || !out || m->magic!=SPMP48_MAGIC || m->head>=SPMP48_EVENT_CAP ||
    m->count>SPMP48_EVENT_CAP || index>=m->count)return 0;
 first=(m->head+SPMP48_EVENT_CAP-m->count)%SPMP48_EVENT_CAP;
 *out=m->entries[(first+index)%SPMP48_EVENT_CAP];
 return 1;
}
