#ifndef __ATA_H__
#define __ATA_H__

/*
 * Драйвер ATA VFS
 */

#include <vfs.h>

vfs_node_t *ata_init();

uint64_t ata_read(vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer);
uint64_t ata_write(vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer);
uint64_t ata_open(vfs_node_t *node, uint32_t flags);
uint64_t ata_close(vfs_node_t *node);

#endif //__ATA_H__
