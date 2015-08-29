#ifndef __FS_H__
#define __FS_H__

#include <vfs.h>

// Корневой узел
extern vfs_node_t *root;

#ifdef __HOSTED__
void fs_init(void *p, int len);
#else
void fs_init(void);
#endif

#endif // __FS_H__
