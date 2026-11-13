/* Only compile when explicitly relinking original user-owned stock .O files.
   These signatures are based on observed ARM arguments r0..r2 for query-only
   routines, not a claim that the full decoder ABI has been recovered.
   NEVER call codec init/run through this exploratory adapter. */
#include <stdint.h>
#if defined(SPMP_RELINK_MP3)
extern void mp3_decoder_working_ram(uint32_t available,uint32_t *a,uint32_t *b);
int spmp_legacy_mp3_query_memory(uint32_t available,uint32_t *a,uint32_t *b){
 if(!a||!b)return -1;
 mp3_decoder_working_ram(available,a,b);
 return 0;
}
#elif defined(SPMP_RELINK_FLAC)
extern uint32_t flac_working_ram_size(void);
uint32_t spmp_legacy_flac_query_memory(void){return flac_working_ram_size();}
#else
#error Must explicitly select SPMP_RELINK_MP3 or SPMP_RELINK_FLAC
#endif
