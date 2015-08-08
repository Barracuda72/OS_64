#include <klibc.h>
#include <mem.h>
#include <dirent.h>
#include <errno.h>

#include <vfs.h>
#include <ext2.h>

vfs_driver_t ext2_drv = {
  ext2_read,
  ext2_write,
  ext2_open,
  ext2_close,
  ext2_readdir,
  ext2_finddir,
  ext2_init,
  ext2_fini
};

uint32_t i_start[16] = {0}; // Начало таблицы inode для группы, индекс = номер группы

/*
 * Список открытых файлов
 * Значение элемента - позиция в файле
 */
uint32_t open_files[1024] = {0xFFFFFFFF};
uint32_t file_inodes[1024] = {0xFFFFFFFF};

vfs_node_t *ext2_init(vfs_node_t *node)
{
  vfs_node_t *root = NULL;

  if (node == NULL)
    return root;

  ext2_superblock *super = kmalloc(sizeof(ext2_superblock));
  vfs_read(node, 0x400, sizeof(ext2_superblock), (uint8_t *)super);

  int res = super->magic == EXT2_MAGIC;

  printf("Is this EXT2? %s\n", res ? "Yes" : "No");
  if (res)
  {
    printf("FS revision %d.%d\n", super->ver_major, super->ver_minor);
    printf("Block size %d\n", 1024<<super->block_size);
    printf("Inode size %d\n", super->inode_size);
    printf("Blocks in group - %d\n", super->blk_in_grp);
    printf("Inodes in group - %d\n", super->ind_in_grp);
    printf("Block number of superblock - %d\n", super->sblock_num);

    root = vfs_alloc_node();
    root->drv = &ext2_drv;
    root->flags = VFS_DIRECTORY;
    root->inode = EXT2_ROOT_INODE;
    root->reserved = (uint64_t)super;
    root->ptr = node;
  } else {
    kfree(super);
  }

  return root;
}

uint64_t ext2_fini(vfs_node_t *node)
{
  if (node == NULL)
    return EINVAL;

  kfree((void *)node->reserved);
  kfree(node);
  return 0;
}

ext2_inode *ext2_read_inode(vfs_node_t *node)
{
  uint64_t inode = node->inode;
  ext2_superblock *super = (ext2_superblock *)node->reserved;

  uint32_t bg = (inode-1)/(super->ind_in_grp);
  uint32_t id = (inode-1)%(super->ind_in_grp);

  uint32_t i_start = 0;
  ext2_group_desc *desc = kmalloc(sizeof(ext2_group_desc));

  vfs_read(node->ptr, 
           (1024<<super->block_size)*(super->sblock_num+1) + 
             bg*sizeof(ext2_group_desc), 
           sizeof(ext2_group_desc),
           (uint8_t *)desc
          );

  i_start = desc->inode_st*(1024<<super->block_size);

  //printf("Таблица inode расположена по адресу 0x%X\n", i_start);
  kfree(desc);

  ext2_inode *in = kmalloc(super->inode_size);
  vfs_read(node->ptr,
           i_start + (id*super->inode_size),
           super->inode_size,
           (uint8_t *)in
          );
  //printf("Тип и разрешения inode %d - 0x%X\n", inode, in->type_perm);
  //printf("Это директория? %s\n", in->type_perm&EXT2_INODE_DIR ? "Да" : "Нет");
  //printf("Размер %d\n", SIZE(in));
  return in;
}

char *ext2_read_inode_data(vfs_node_t *node, ext2_inode *in)
{
  int sz = 0;
  ext2_superblock *super = (ext2_superblock *)node->reserved;
  char *data = kmalloc(12*BLKSZ(super));
  /*
   * Считываем все данные из блоков "прямого" доступа
   * Это даст возможность читать файлы по 12 килобайт
   * Потом, конечно, надо будет сделать все по уму
   */
  for (sz = 0; sz < 12; sz++)
  {
    if (in->direct[sz] == 0)
      break;
    //printf("Используется блок %d (0x%X)\n", in->direct[sz], in->direct[sz]*(1024<<super->block_size)+start);
    vfs_read(node->ptr, 
             in->direct[sz]*BLKSZ(super),
             BLKSZ(super), 
             data + sz*BLKSZ(super)
            );
  }
  //printf("Считано %d блоков\n", sz);
  return data;
}

uint64_t ext2_open(vfs_node_t *node, uint32_t flags)
{
  return 0;
}
#if 0
int _ext2_open(const char *path, int flags)
{
  if (!start)
    return;
  printf("Попытка открыть %s\n", path);
  int i;
  for (i = 0; (i < 1024) && (open_files[i] != 0xFFFFFFFF); i++);
      
  if (i == 1024)
    return EMFILE;
    
  /* Свободный id нашелся, ищем файл */
  int inode = EXT2_ROOT_INODE;
  int our_inode = 0;
  while (our_inode == 0)
  {
    ext2_inode *in = ext2_read_inode(inode);
    if ((in->type_perm&EXT2_INODE_DIR) != EXT2_INODE_DIR)
    {
      free(in);
      return ENOTDIR;
    }
    
    //printf("Path: %s\n", path);
    while (*path == '/') path++;
    int s = 0;
    while ((path[s] != '/') && (path[s] != 0)) s++;
    
    char *data = ext2_read_inode_data(in);
    ext2_direntry *dirent = 0;
    char *data2 = data;
    inode = 0;
    for (; (data2 - data) < in->size_low; data2 += dirent->size)
    {
      dirent = (ext2_direntry *)data2;
      if (dirent->lenght == 0)
      {
        break;
      }
      //printf("Direntry: %s\n", dirent->name);
      if (!strncmp(dirent->name, path, s))
      {
        //printf("BRK;\n");
        if (path[s] == '/') // Это не последний элемент пути
        {
          inode = dirent->inode;
        } else {
          our_inode = dirent->inode;
        }
        break;
      }
    }
    free(in);
    free(data);
    if ((inode == 0)&&(our_inode == 0))
      return ENOENT;
    path += s;
  }
  open_files[i] = 0;
  file_inodes[i] = our_inode;
  return i;
}
#endif
uint64_t ext2_read(vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer)
{
  if (node == NULL)
    return EINVAL;

  ext2_inode *in = ext2_read_inode(node);
  if (offset > SIZE(in))
    return EINVAL;

  if ((offset + size) > SIZE(in))
    size = SIZE(in) - offset;

  uint8_t *data = ext2_read_inode_data(node, in);
  memcpy(buffer, data + offset, size);
  kfree(data);
  return size;
}
#if 0
ssize_t _ext2_read( int fd, void *buf, size_t count )
{
  if (!start)
    return EINVAL;

  if (count == 0)
    return 0;
    
  if(fd > 1024)
    return EBADF;
    
  if((open_files[fd] == 0xFFFFFFFF) || 
     (file_inodes[fd] == 0xFFFFFFFF))
    return EBADF;
    
  if(buf == NULL)
    return EINVAL;  
    
  uint32_t pos = open_files[fd];
  ext2_inode *in = ext2_read_inode(file_inodes[fd]);
  if(pos >= in->size_low)
  {
    free(in);
    return 0;
  }
  
  if((pos + count) > in->size_low)
  {
    count = in->size_low - pos;
  }
  
  uint32_t cnt = 0;
  char *data = (char *)buf;
  if (pos < (1024<<super->block_size)*12)
  {
    // Считываем данные из Direct Pointers
    int i = pos>>(10+super->block_size);
    int k;
    for(; (i < 12) && (count > 0); i++)
    {
      // Сколько нужно считать из текущего блока
      k = (count > (1024<<super->block_size) ? (1024<<super->block_size) : count);
      k = k > ((1024<<super->block_size) - (pos%(1024<<super->block_size))) ? 
        ((1024<<super->block_size) - (pos%(1024<<super->block_size))) : k;
      fseek(f,
            start + in->direct[i]*(1024<<super->block_size) + (pos%(1024<<super->block_size)), 
            SEEK_SET);
      fread(data + cnt,
            k,
            1,
            f);
      cnt += k;
      count -= k;
      pos += k;
    }
  }
  free(in);
  open_files[fd] = pos;
  return cnt;
}
#endif
uint64_t ext2_write(vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer)
{
  return EINVAL;
}

uint64_t ext2_close(vfs_node_t *node)
{
  return 0;
}

struct dirent *ext2_readdir(vfs_node_t *node, uint64_t index)
{
  if (node == NULL)
    return EINVAL;

  int i;

  ext2_inode *in = ext2_read_inode(node);
  char *data = ext2_read_inode_data(node, in);

  char *e_d = data;
  ext2_direntry *ed = (ext2_direntry *)data;

  struct dirent *d = kmalloc(sizeof(struct dirent));

  for (i = 0; 
       ((((uint64_t)ed) - ((uint64_t)data)) < SIZE(in)) &&
         (ed->length != 0) &&
         (i < index);
       ed = (ext2_direntry *)(e_d += ed->size)
      )
    i++;

  if ((i == index) && 
      ((((uint64_t)ed) - ((uint64_t)data)) < SIZE(in)) &&
      (ed->length != 0)
     )
  {
    strncpy(d->name, ed->name, ed->length);
    d->name[ed->length] = 0;
    d->inode = ed->inode;
  } else {
    kfree(d);
    d = (struct dirent *)EINVAL;
  }

  kfree(in);
  kfree(data);

  return d;
}

vfs_node_t *ext2_finddir(vfs_node_t *node, char *name)
{
  if (node == NULL)
    return EINVAL;

  ext2_inode *in = ext2_read_inode(node);
  char *data = ext2_read_inode_data(node, in);

  char *e_d = data;
  ext2_direntry *ed = (ext2_direntry *)data;

  vfs_node_t *v = vfs_alloc_node();

  int f;

  for (f = 0; 
       ((((uint64_t)ed) - ((uint64_t)data)) < SIZE(in)) &&
         (ed->length != 0);
       ed = (ext2_direntry *)(e_d += ed->size)
      )
  {
    if (!strncmp(name, ed->name, ed->length))
    {
      f = 1;
      break;
    }
  }

  if (f == 1)
  {
    strncpy(v->name, ed->name, ed->length);
    v->inode = ed->inode;
    v->drv = node->drv;
    v->reserved = node->reserved;
    v->ptr = node->ptr;

    ext2_inode *fi = ext2_read_inode(v);

    v->rights = fi->type_perm;
    v->uid = fi->uid;
    v->gid = fi->gid;
    v->length = fi->size_low; // FIXME!
    kfree(fi);
    // FIXME: здесь нужно считывать сам inode и данные из него 
    // переносить в запись
  } else {
    kfree(v);
    v = NULL;
  }

  kfree(in);
  kfree(data);

  return v;
}

/*
off_t ext2_lseek( int fd, off_t offset, int whence )
{
  if (!start)
    return EINVAL;

  if(fd > 1024)
    return EBADF;
    
  if((open_files[fd] == 0xFFFFFFFF) || 
     (file_inodes[fd] == 0xFFFFFFFF))
    return EBADF;
    
  if( (whence != SEEK_CUR)
     && (whence != SEEK_SET)
     && (whence != SEEK_END))
    return EINVAL;
    
  uint32_t pos = open_files[fd];
  ext2_inode *in = ext2_read_inode(file_inodes[fd]);
    
  if (whence == SEEK_SET)
    pos = offset;
    
  if (whence == SEEK_CUR)
    pos += offset;
    
  if (whence == SEEK_END)
    pos = offset + in->size_low;
    
  if ((pos > in->size_low) || (pos < 0))
  {
    free(in);
    return EINVAL;
  }
  free(in);
  open_files[fd] = pos;
  return pos;
}
*/

