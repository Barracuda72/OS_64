#ifndef __FAT32_H__
#define __FAT32_H__

#include <vfs.h>

int fat32_init(vfs_node_t *node);
int fat32_fini(vfs_node_t *node);

#endif //__FAT32_H__
