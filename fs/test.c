/*
 * Это - площадка для проверки драйвера файловой системы
 * в hosted-режиме
 */

#include <vfs.h>
#include <fs.h>
#include <initrd.h>
#include <devfs.h>
#include <kprintf.h>
#include <mem.h>
#include <elf.h>
#include <errno.h>

#include <stddef.h>

#ifndef __HOSTED__
#include <ata.h>
#include <debug.h>
#endif

void fs_print_dir(vfs_node_t *dir)
{
  int i = 0;
  struct dirent *de = NULL;
  //BREAK();
  do 
  {
    de = vfs_readdir(dir, i);
    if (de != EBADF && de != EINVAL && de != NULL)
    {
      kprintf ("Found file %s\n", de->name);
      kfree(de);
    } else {
      break;
    }
    i++;
  } while (1);
  kprintf("---\n");
}

int start_process(void *buffer);

void fs_test_main()
{
  // Распечатаем содержимое devfs
  int i = 0;
  struct dirent *node = 0;
  vfs_node_t *fsnode;

  if (root->ptr != NULL)
  {
    kprintf("Root mounted successfully\n");
    kprintf("Contents:\n");
    fs_print_dir(root);

    char *test_file = "test.elf"; //"hello.txt";

    kprintf("Contents of %s:\n", test_file);
    vfs_node_t *hello = vfs_finddir(root, test_file);
    if (hello != NULL)
    {
      char *buf = kmalloc(10000);
      int sz = vfs_read(hello, 0, 10000, buf);
      if (sz < 0)
        kprintf("Something wrong, return code %d\n", sz);
      else {
        kprintf("(Readed %d bytes)\n", sz);
        start_process(buf);
    /*    int j;
        for (j = 0; j < sz; j++)
          kprintf("%c", buf[j]);
*/
        kprintf("\"\n");
      }
      kfree(buf);
    } else {
      kprintf("File not found!\n");
    }
  }

#ifdef __HOSTED__
  vfs_umount(root);
#endif
/*
  kfree(fsnode);
  kfree(root);
  kfree(dfs);
  kfree(ird);
*/
}

