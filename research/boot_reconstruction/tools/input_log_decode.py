#!/usr/bin/env python3
"""Summarize input-test CSV as per-session candidate MMI button mappings.
Prompt labels are operator instructions, not independent physical identification.
"""
import argparse,csv,json,re
from pathlib import Path
HEX=re.compile(r'^[0-9A-Fa-f]{8}$')
def decode(text,device_label=None):
 sessions=[];current=None;skipped=0
 for line in text.splitlines():
  if line.startswith('#SPMP_INPUT_TEST_V1,'):
   current={'session_index':len(sessions)+1,'metadata':line,'candidate_mappings':{},'events':[],'ended':False}
   sessions.append(current);continue
  if not line or line.startswith('event,') or line.startswith('#'):continue
  if len(line)>512:skipped+=1;continue
  try:fields=next(csv.reader([line]))
  except csv.Error:skipped+=1;continue
  if not current or len(fields)!=7 or fields[0] not in ('ARM','PROMPT','KEY','SAMPLE','END','END_ERROR','MENU') or not all(HEX.fullmatch(fields[i]) for i in (1,2,4,5,6)):
   skipped+=1;continue
  kind,poll,time,prompt,mask,pressed,released=fields
  if prompt not in ('UP','DOWN','LEFT','RIGHT','A','B','X','Y','L','R','START','SELECT','BACK','FREE'):
   skipped+=1;continue
  record={'event':kind,'poll':int(poll,16),'time_units':int(time,16),'prompt':prompt,
          'mask':'0x'+mask.upper(),'pressed':'0x'+pressed.upper(),'released':'0x'+released.upper()}
  current['events'].append(record)
  if kind=='SAMPLE' and int(mask,16):
   values=current['candidate_mappings'].setdefault(prompt,[])
   if record['mask'] not in values:values.append(record['mask'])
  if kind in ('END','END_ERROR'):current['ended']=True;current['end_event']=kind
 return {'format':'SPMP_INPUT_TEST_V1','device_label_supplied_by_user':device_label,'sessions':sessions,
  'ignored_or_partial_lines':skipped,'limits':'Candidate MMI masks associated with requested prompts. No GPIO or physical identity is inferred.'}
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('csv',type=Path);p.add_argument('--device');p.add_argument('--output',type=Path);a=p.parse_args()
 with a.csv.open('rb') as f:data=f.read(2*1024*1024+1)
 if len(data)>2*1024*1024:p.error('Input limit: 2 MiB')
 result=decode(data.decode('ascii',errors='replace'),a.device);text=json.dumps(result,indent=2,ensure_ascii=False)+'\n'
 if a.output:a.output.write_text(text)
 else:print(text,end='')
