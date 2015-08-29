/*
 * Это - площадка для проверки драйвера файловой системы
 * в hosted-режиме
 */

#include <vfs.h>
#include <initrd.h>
#include <devfs.h>
#include <kprintf.h>
#include <mem.h>
#include <elf.h>
#include <errno.h>

#ifndef __HOSTED__
#include <ata.h>
#include <debug.h>
#endif

// Корневой узел
vfs_node_t *root;

// Узел DevFS
vfs_node_t *dfs;

void fs_print_dir(vfs_node_t *p);

#ifdef __HOSTED__
void fs_init(void *p, int len)
#else
void fs_init(void)
#endif
{
  char *rnn;

  kprintf("FS initialization...\n");
  vfs_node_t *main;

#ifdef __HOSTED__
  main = initrd_init(p, len);
  rnn = "initrd0";
#else
  main = ata_init();
  rnn = "ata0";
#endif

  dfs = devfs_init();

  devfs_add(main);

  root = vfs_alloc_node();
  root->name[0] = '/';
  root->name[1] = 0;

  root->flags = VFS_DIRECTORY;

  fs_print_dir(dfs);

  vfs_node_t *t = vfs_finddir(dfs, rnn);
  vfs_mount(t, root);

  if (root->ptr != NULL)
  {
    kprintf("Root mounted successfully\n");
  }
}

