/*
 * Драйвер терминального устройства
 *
 * Объединяет в себе два драйвера: для клавиатуры и для экрана
 *
 */

#ifndef __TTY_H__
#define __TTY_H__

#include <stdint.h>
#include <vfs.h>

uint64_t tty_read(vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer);
uint64_t tty_write(vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer);
uint64_t tty_open(vfs_node_t *node, uint32_t flags);
uint64_t tty_close(vfs_node_t *node);

vfs_node_t *tty_init(vfs_node_t *node);
uint64_t tty_fini(vfs_node_t *node);

#endif // __TTY_H__
