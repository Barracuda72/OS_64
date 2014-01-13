#include <stdint.h>
#include <vfs.h>
#include <fat32.h>
#include <errno.h>

vfs_node_t *fat32_init(vfs_node_t *node)
{
  return NULL;
}

int fat32_fini(vfs_node_t *node)
{
  return EINVAL;
}

