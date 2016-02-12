/*
 * sys_write.c
 *
 * Системный вызов write()
 *
 */

#include <fs.h>
#include <task.h>
#include <syscall.h>
#include <errno.h>

#include <stddef.h>

int sys_write(int file, const char *ptr, int len) 
{
  if (file > MAX_OPEN_FILES || file < 0)
    return EBADF;

  fs_open_desc *f = curr_cpu_task->files[file];

  if (f == NULL)
    return EBADF;

  int res = vfs_write(f->node, f->offset, len, ptr);

  if (!(f->node->flags&VFS_TTY) && !(f->node->flags&VFS_PIPE))
    f->offset += res;

  return res;
}
