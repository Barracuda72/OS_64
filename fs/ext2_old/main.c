#include <stdio.h>
#include <stdlib.h>

#include "part.h"
#include "ext2.h"

int main()
{
#if 0
  FILE *f = fopen("image.ext2", "rb");
  part *p = malloc(sizeof(part));
  fread(p, sizeof(part), 1, f);
  fclose(f);

  int i,j,k;
  for ( i = 0; i < 4; i++)
  {
  /*  j = (p->parts[i].s_head << 16) | (p->parts[i].s_sector << 10) | (p->parts[i].s_track);
    k = (p->parts[i].e_head << 16) | (p->parts[i].e_sector << 10) | (p->parts[i].e_track);
    printf("Partition %d, type %x, first sec %d\n",
            i,
            p->parts[i].type,
            p->parts[i].first_sector
          );*/
    if ( p->parts[i].type == 0x83 ) // Кажется, нашли
      break;
  }

  if ( i == 4 )
  {
    free(p);
    printf("No linux partition found!\n");
    return -1;
  }

  int start = (p->parts[i].first_sector<<9);
  free(p);

  f = fopen("image.ext2", "rb");

  fseek(f, start + 0x400, SEEK_SET);
  ext2_superblock *super = malloc(sizeof(ext2_superblock));
  fread(super, sizeof(ext2_superblock), 1, f);

  printf("Это EXT2? %s\n", super->magic == EXT2_MAGIC ? "Да" : "Нет");
  printf("Версия ФС %d.%d\n", super->ver_major, super->ver_minor);
  printf("Размер блока %d\n", 1024<<super->block_size);
  printf("Размер inode %d\n", super->inode_size);
  printf("Количество блоков в группе - %d\n", super->blk_in_grp);
  printf("Количество inode в группе - %d\n", super->ind_in_grp);
  printf("Номер блока, содержащего суперблок - %d\n", super->sblock_num);

  ext2_group_desc *desc = malloc(sizeof(ext2_group_desc));
  fseek(f, start + (1024<<super->block_size)*(super->sblock_num+1), SEEK_SET);
  fread(desc, sizeof(ext2_group_desc), 1, f);
  int i_start = desc->inode_st*(1024<<super->block_size);
  printf("Таблица inode расположена по адресу %d (0x%X)\n", i_start, i_start);
  ext2_inode *in = malloc(super->inode_size);
  fseek(f, i_start + start + ((2 - 1)*super->inode_size), SEEK_SET);
  fread(in, super->inode_size, 1, f);
  printf("Тип и разрешения inode 2 - 0x%X\n", in->type_perm);
  printf("Это директория? %s\n", in->type_perm&EXT2_INODE_DIR ? "Да" : "Нет");
  printf("Размер %d\n", in->size_low);

  int sz = 0;
  char *dir_data = 0;
  for (sz = 0; sz < 12; sz++)
  {
    if (in->direct[sz] == 0)
      break;
    printf("Используется блок %d (0x%X)\n", in->direct[sz], in->direct[sz]*(1024<<super->block_size)+start);
    dir_data = realloc(dir_data, (sz + 1)*(1024<<super->block_size));
    fseek(f, start + in->direct[sz]*(1024<<super->block_size), SEEK_SET);
    fread(dir_data + sz*(1024<<super->block_size), 1024<<super->block_size, 1, f);
  }
  printf("Считано %d блоков\n", sz);
  sz *= (1024<<super->block_size);

  ext2_direntry *dirent = dir_data;
  char *dir_data2 = dir_data;

  char tmp[256];
  int inode_index = 0;
  for (; (dir_data2 - dir_data) < sz; dir_data2 += dirent->size)
  {
    dirent = dir_data2;
    if (dirent->lenght == 0)
      break;
    strncpy(tmp, dirent->name, dirent->lenght);
    tmp[dirent->lenght] = 0;
    printf("Найден файл %s, %d\n", tmp, dirent->lenght);
    if (!strncmp(tmp, "hello.txt", 10))
    {
      inode_index = dirent->inode;
      break;
    }
  }

  if (inode_index != 0)
  {
    fseek(f, i_start + start + ((inode_index - 1)*super->inode_size), SEEK_SET);
    fread(in, super->inode_size, 1, f);
    printf("Тип и разрешения inode - 0x%X\n", in->type_perm);
    printf("Это директория? %s\n", in->type_perm&EXT2_INODE_DIR ? "Да" : "Нет");
    printf("Размер %d\n", in->size_low);
    char *file_data = 0;
    for (sz = 0; sz < 12; sz++)
    {
      if (in->direct[sz] == 0)
        break;
      printf("Используется блок %d (0x%X)\n", in->direct[sz], in->direct[sz]*(1024<<super->block_size)+start);
      file_data = realloc(file_data, (sz + 1)*(1024<<super->block_size));
      fseek(f, start + in->direct[sz]*(1024<<super->block_size), SEEK_SET);
      fread(file_data + sz*(1024<<super->block_size), 1024<<super->block_size, 1, f);
    }
    printf("Считано %d блоков\n", sz);
    printf("Содержимое файла:\n%s\n", file_data);
    free(file_data);
  }

  fclose(f);
  free(super);
  free(desc);
  free(in);
  free(dir_data);
#endif

  ext2_init();
  int i = ext2_open("/home/barracuda/help.txt", 0);
  if (i < 0)
    return 0;
  char *buf = malloc(1024);
  int j,k;
  for(j = 0; j < 512; j++)
  {
    if((k = ext2_read(i, buf, 2)) == 0)
      break;
    buf[k] = 0;
    printf("%s", buf);
    //ext2_lseek(i, 3, SEEK_CUR);
  }
  free(buf);
  ext2_close(i);
  ext2_fini();
}

