#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
typedef unsigned int dd;
typedef unsigned short dw;
typedef unsigned char db;
typedef struct {uint8_t keys[128];} ge_key_data_t;
typedef struct {int dummy;} mcatch_loadimg_t;
FILE *zlog;
unsigned NativeGE_getTime(void){return 0;}

void ControlSWCHAWrite(void);
void srv_print(char*);
void QueueSoundBytes(void);
void TestLightgunHit(int,int);
void srv_SetPalette(void);
void ClearScreenBuffers(void);
void srv_Cleanup(void);
void cli_LoadNextStarpath(void);
void cli_ReloadStarpath(void);
void set_status(char *);
void Init_Service(void);
void Controls(void);
void srv_Events(void);
void srv_CopyScreen(void);
int srv_done;

#include "globals.c"
#include "ct.c"
#include "carts.c"
#include "kidvid.c"
#include "palette.c"
#include "position.c"
#include "2600core.c"
