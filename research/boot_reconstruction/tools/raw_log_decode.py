#!/usr/bin/env python3
"""Decode SPMP RAW V2 snapshots. Preserve layers; do not infer GPIO/button codes."""
import argparse,csv,json,re
from pathlib import Path
HEX=re.compile(r'^[0-9A-Fa-f]{8}$')
PROMPTS={'ARM','UP','DOWN','LEFT','RIGHT','A','B','X','Y','L','R','START','SELECT','HOLD_SHORT','VOL_PLUS','VOL_MINUS','SELECT_VOL_PLUS','SELECT_VOL_MINUS','HOLD_LOCK','HOLD_UNLOCK','POWER_SHORT','FREE'}
FIELDS=['poll','time_units','cooked','pressed','released','state_before','event_before','state_after','event_after','call_units']
def decode(text,device_label=None):
 sessions=[];current=None;skipped=0
 for line in text.splitlines():
  if len(line)>512:skipped+=1;continue
  if line.startswith('#SPMP_RAW_INPUT_V2,'):
   current={'session_index':len(sessions)+1,'metadata':line,'events':[],'ended':False,'observed_masks_by_prompt':{}}
   sessions.append(current);continue
  if not line or line.startswith('#') or line.startswith('event,'):continue
  try:f=next(csv.reader([line]))
  except csv.Error:skipped+=1;continue
  if not current or len(f)!=12 or f[0] not in ('PRE','SAMPLE','TICK','PROMPT','GAP','END','END_ERROR') or f[3] not in PROMPTS:
   skipped+=1;continue
  numbers=f[1:3]+f[4:]
  if not all(HEX.fullmatch(v) or v=='NA' for v in numbers) or any(numbers[i]=='NA' for i in (0,1,5,6,9)):
   skipped+=1;continue
  # NA is mandatory for pending outputs in PRE; only PRE can lack them.
  if f[0]=='PRE' and any(f[i]!='NA' for i in (4,5,6,9,10)) or f[0]!='PRE' and 'NA' in numbers:
   skipped+=1;continue
  record={'event':f[0],'prompt':f[3]}
  for name,value in zip(FIELDS,numbers):record[name]=None if value=='NA' else int(value,16) if name in ('poll','time_units','call_units') else '0x'+value.upper()
  current['events'].append(record)
  if f[0] in ('PRE','SAMPLE') and f[3] not in ('ARM','FREE'):
   observed=current['observed_masks_by_prompt'].setdefault(f[3],{})
   for name in ('cooked','state_before','event_before','state_after','event_after'):
    value=record[name]
    if value and int(value,16):
     values=observed.setdefault(name,[])
     if value not in values:values.append(value)
  if f[0] in ('END','END_ERROR'):current['ended']=True;current['end_event']=f[0]
 return {'format':'SPMP_RAW_INPUT_V2','device_label_supplied_by_user':device_label,'sessions':sessions,'ignored_or_partial_lines':skipped,
  'limits':'Observed MMI RAM/cooked values per operator prompt. Non-atomic snapshots, no automatic physical/GPIO map. PRE after/cooked fields are unknown.'}
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('csv',type=Path);p.add_argument('--device');p.add_argument('--output',type=Path);a=p.parse_args()
 with a.csv.open('rb') as f:data=f.read(2*1024*1024+1)
 if len(data)>2*1024*1024:p.error('Input limit: 2 MiB')
 result=decode(data.decode('ascii',errors='replace'),a.device);text=json.dumps(result,indent=2,ensure_ascii=False)+'\n'
 if a.output:a.output.write_text(text)
 else:print(text,end='')
