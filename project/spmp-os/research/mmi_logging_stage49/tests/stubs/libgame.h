/* Compilation test only; not OEM or upstream header; no actual ABI runtime. */
#ifndef LIBGAME_STUB_H
#define LIBGAME_STUB_H
#define _ECOS_O_RDONLY (1<<0)
#define _ECOS_O_WRONLY (1<<1)
#define _ECOS_O_CREAT (1<<3)
#define _ECOS_O_APPEND (1<<7)
void libgame_init(void);
extern int (*_ecos_open)(const char*,int,int);
extern int (*_ecos_write)(int,const void*,unsigned int);
extern int (*_ecos_read)(int,void*,unsigned int);
extern int (*_ecos_close)(int);
#endif
