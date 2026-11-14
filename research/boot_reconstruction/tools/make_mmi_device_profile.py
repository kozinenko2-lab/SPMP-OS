#!/usr/bin/env python3
"""Export the explicitly selected per-console raw-state profile to portable C."""
import argparse,json
from pathlib import Path
LABELS=['UP','DOWN','LEFT','RIGHT','A','B','X','Y','L','R','START','SELECT','VOL_PLUS','VOL_MINUS']
def produce(profile):
 values=profile['raw_state_layer']['candidate_mappings'];masks=[int(values[k],16) for k in LABELS]
 if any(not x or x&(x-1) for x in masks) or len(set(masks))!=len(masks):raise ValueError('Distinct measured single-bit state masks required')
 power=int(profile['raw_event_layer']['candidate_mappings']['POWER_SHORT'],16)
 if not power or power&(power-1):raise ValueError('Single-bit POWER event required')
 out=['/* Generated from this console user log; not a universal SPMP8000 map. */',
  '/* Source CSV SHA256: '+profile['raw_source_log']['sha256']+' */',
  '#ifndef SPMP_MMI_USER01_PROFILE_H','#define SPMP_MMI_USER01_PROFILE_H','#include "spmp_mmi_input.h"',
  'static const struct spmp_mmi_profile spmp_mmi_user01_profile={ {']
 out += [' '+hex(value)+'u, /* '+label+' */' for label,value in zip(LABELS,masks)]
 return '\n'.join(out+[' },'+hex(power)+'u};','#endif',''])
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('json',type=Path);p.add_argument('--header',type=Path,required=True);a=p.parse_args();a.header.write_text(produce(json.loads(a.json.read_text())))
