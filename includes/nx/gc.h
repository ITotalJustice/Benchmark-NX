#ifndef _GC_H_
#define _GC_H_


#include <stdbool.h>


//
bool init_gc(void);

//
void exit_gc(void);

//
bool poll_gc(void);

//
bool mount_gc(void);

//
bool mount_secure_gc(void);

//
bool unmount_gc(void);

//
bool unmount_secure_gc(void);

//
const char *return_gc_mount_path(void);

#endif