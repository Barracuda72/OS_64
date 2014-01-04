#ifndef __INITRD_H__
#define __INITRD_H__

/*
 * INITal Ram Disk
 */

#include <vfs.h>

vfs_node_t *initrd_init(void *p, uint64_t size);

uint64_t initrd_read(vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer);
uint64_t initrd_write(vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer);
uint64_t initrd_open(vfs_node_t *node, uint32_t flags);
uint64_t initrd_close(vfs_node_t *node);

#endif //__INITRD_H__
