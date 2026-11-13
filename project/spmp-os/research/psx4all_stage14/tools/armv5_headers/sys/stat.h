#pragma once
struct stat { unsigned long st_mode; unsigned long st_size; };
#ifdef __cplusplus
extern "C" {
#endif
int stat(const char *, struct stat *);
int mkdir(const char *, unsigned long);
#ifdef __cplusplus
}
#endif
