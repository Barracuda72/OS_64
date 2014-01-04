#ifndef __DEVFS_H__
#define __DEVFS_H__

/*
 * DEVice File System
 * Файловая система, представляющая устройства
 * Обычно монтируется на /dev
 */

#include <vfs.h>

vfs_node_t *devfs_init();

struct dirent *devfs_readdir(vfs_node_t *node, uint64_t index);
vfs_node_t *devfs_finddir(vfs_node_t *node, char *name);
int devfs_add(vfs_node_t *node);

#endif //__DEVFS_H__
