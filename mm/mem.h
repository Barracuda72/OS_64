#ifndef __MEM_H__
#define __MEM_H__

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
// Заголовок кусочка памяти в куче
typedef struct _kmem_header
{
  unsigned int magic;	// Магическое число
  unsigned int size : 31;	// Размер кусочка
  unsigned int free : 1;
  struct _kmem_header *prev;	// Указатель на предыдущий кусочек
} kmem_header __attribute((packed));

void mem_init(unsigned long pl_addr, unsigned long pl_size);

void *kmalloc(unsigned long size);
void kfree(void *p);
#endif //__MEM_H__
