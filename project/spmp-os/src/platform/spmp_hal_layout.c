#include "spmp_hal_layout.h"
#include <stdint.h>
static int valid(spmp_phys_region p){
 return p.size && (uint64_t)p.start+(uint64_t)p.size<=UINT32_MAX;
}
static int within(spmp_phys_region t,spmp_phys_region p){
 return valid(t)&&valid(p)&&(uint64_t)p.start>=t.start&&
  (uint64_t)p.start+p.size <= (uint64_t)t.start+t.size;
}
static int overlap(spmp_phys_region a,spmp_phys_region b){
 return (uint64_t)a.start<(uint64_t)b.start+b.size&&
  (uint64_t)b.start<(uint64_t)a.start+a.size;
}
int spmp_hal_check_layout(const spmp_hal_layout *l){
 if(!l||!valid(l->total)||!within(l->total,l->redboot_reserved))return -1;
 const spmp_phys_region p[]={l->os,l->core_slot,l->game,l->video,l->audio};
 for(unsigned i=0;i<sizeof(p)/sizeof(p[0]);i++){
  if(!within(l->total,p[i])||overlap(l->redboot_reserved,p[i]))return -1;
  for(unsigned j=0;j<i;j++)if(overlap(p[j],p[i]))return -1;
 }
 return 0;
}
