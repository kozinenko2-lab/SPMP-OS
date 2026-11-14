/* SPDX-License-Identifier: MIT */
#ifndef SPMP_FILE_COPY_H
#define SPMP_FILE_COPY_H
#include <stdint.h>
#define SPMP_COPY_PATH 512u
#define SPMP_COPY_BLOCK 2048u
#define SPMP_COPY_STAGE_TRIES 16u
struct spmp_copy_meta { uint32_t mode, bytes, mtime; }; /* mode: 8 regular, 1 directory */
/* SD-relative paths under SPMP-OS/User only. Negative errno, fd >= 0.
 * All callbacks are mandatory. create_exclusive must NEVER open an existing
 * name. publish_noreplace must atomically fail if the destination exists.
 * close consumes the handle even on error. unlink removes only our owned stage.
 * Callbacks must enforce the SD root, reject symlinks/other volumes and serialize
 * access and pin media identity for the task lifetime; this interface is NOT a
 * binding to any guessed OEM address.
 * read/write return bytes; seek_zero returns 0 on success. */
struct spmp_copy_io {
 void *user;
 int (*stat)(void *,const char *,struct spmp_copy_meta *);
 int (*fstat)(void *,int,struct spmp_copy_meta *);
 int (*open_read)(void *,const char *);
 int (*create_exclusive)(void *,const char *);
 int (*read)(void *,int,void *,uint32_t);
 int (*write)(void *,int,const void *,uint32_t);
 int (*seek_zero)(void *,int);
 int (*close)(void *,int);
 int (*publish_noreplace)(void *,const char *,const char *);
 int (*unlink_stage)(void *,const char *);
};
enum spmp_copy_phase {
 SPMP_COPY_IDLE, SPMP_COPY_CHECK, SPMP_COPY_OPEN_SOURCE, SPMP_COPY_CREATE,
 SPMP_COPY_READ, SPMP_COPY_WRITE, SPMP_COPY_SOURCE_EOF, SPMP_COPY_CLOSE_WRITER,
 SPMP_COPY_OPEN_VERIFY, SPMP_COPY_REWIND, SPMP_COPY_VERIFY_SOURCE,
 SPMP_COPY_VERIFY_TARGET, SPMP_COPY_VERIFY_SOURCE_EOF, SPMP_COPY_VERIFY_TARGET_EOF,
 SPMP_COPY_RECHECK, SPMP_COPY_CLOSE_READERS, SPMP_COPY_PUBLISH,
 SPMP_COPY_DONE, SPMP_COPY_FAILED, SPMP_COPY_CANCELLED
};
struct spmp_file_copy {
 struct spmp_copy_io io;
 char source[SPMP_COPY_PATH],destination[SPMP_COPY_PATH],stage[SPMP_COPY_PATH];
 uint8_t block[SPMP_COPY_BLOCK],verify[SPMP_COPY_BLOCK];
 struct spmp_copy_meta original;
 uint32_t written,verified,chunk,used,token,attempts;
 int source_fd,writer_fd,verify_fd,error,cleanup_error;
 enum spmp_copy_phase phase;
 uint8_t owned_stage,cancel_requested,published;
};
int spmp_file_copy_init(struct spmp_file_copy *,const struct spmp_copy_io *);
/* Inputs are copied. No IO in start; an active task cannot be replaced. */
int spmp_file_copy_start(struct spmp_file_copy *,const char *,const char *,uint32_t token);
/* 0 active, 1 terminal. At most one 2-KiB transfer per call. IO itself can block.
 * Cancellation/error closes handles and tries to remove only the owned stage.
 * error and cleanup_error are distinct; owned_stage reports an orphan to retain.
 * Success means read-back comparison + publication, NOT power-loss durability. */
int spmp_file_copy_step(struct spmp_file_copy *);
void spmp_file_copy_cancel(struct spmp_file_copy *);
/* Retry removal of an orphan after a terminal error. Keeps the primary error;
 * clears owned_stage/cleanup_error on success. No reset of an active task. */
int spmp_file_copy_cleanup(struct spmp_file_copy *);
int spmp_file_copy_path_valid(const char *);
#endif
