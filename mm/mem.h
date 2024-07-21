#ifndef __MEM_H__
#define __MEM_H__

#include <stdint.h>
#include <multiboot.h>
#define KMEM_MAGIC 0xF0B0DEFC

/*
 * Идея такая:
 * При инициализации помещаем заголовок с размером, равным
 * размеру heap (~1 Gb) и помеченным как свободный.
 * В дальнейшем при выделении памяти берем первый свободный
 * кусочек, отрезаем от него столько, сколько нужно, и 
 * возвращаем.
 * При освобождении - интереснее: просматриваем предыдущий
 * (для этого и заведено поле prev) и последующий (вычисляем
 * через size) куски, и, если они свободны - объединяем их.
 */
#pragma pack(1)
// Заголовок кусочка памяти в куче
typedef struct __attribute((packed)) _kmem_header
{
  uint32_t magic;  // Магическое число
  uint32_t size : 31;  // Размер кусочка
  uint32_t free : 1;
  struct _kmem_header *prev;  // Указатель на предыдущий кусочек
} kmem_header; 
#pragma pack()

void mem_init(uint64_t pl_addr, uint64_t pl_size, multiboot_info_t *m);

void *kmalloc(uint64_t size);
void kfree(void *p);
#endif //__MEM_H__

