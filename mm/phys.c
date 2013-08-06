#include <phys.h>
#include <page.h>

#include <debug.h>

/*
 * Я уже сам хрен знает чего тут начудил.
 * Но алгоритмы, судя по всему, рабочие.
 * Однако, они построены с учетом 32-битной системы.
 *
 * Принцип работы будет таким:
 * Мы на входе функции инициализации получаем адрес последней
 * свободной физической (да и логической) страницы и размер памяти
 * в байтах (наверное). После этого мы резервируем несколько страниц
 * под битовую карту, помечая в ней занятые области (от 0x100000 до
 * конца ядра). Остальные будут свободны. Поиск свободной страницы
 * будет начинаться с 1-го мегабайта, так как в 0-ом у нас будет
 * память для DMA и прочей фигни. Вроде все.
 * Битовая карта для разметки памяти была выбрана из-за ее простоты
 * и скорости работы вкупе с не таким уж большим расходом памяти.
 * Выделение страницы - O(N) (ускоряется благодаря проверке 64-х 
 * страниц разом);
 * Освобождение - O(1)
 * Занятая память - 1/32768 (1 байт хранит информацию о 32768 байтах;
 * в реальных цифрах это означает, что для хранения информации о
 * памяти размером, скажем, в 4 гигабайта, потребуется 128 килобайт
 * памяти! Не так уж и много).
 * Недостаток алгоритма - сложно искать области нескольких подряд
 * идущих страниц (можно, как компромисс, реализовать "тупой" 
 * алгоритм, который не будет учитывать границы 64-страничных 
 * участков, и по скорости будет не хуже поиска одной страницы).
 */

unsigned long *phys_map;        //Карта физической памяти

unsigned long phys_size;

void *alloc_phys_page()
{
        unsigned long i,j,k,tmp;
	// Начинаем поиски с первого мегабайта
        for (i = (1<<2); (i < phys_size) 
                    &&(phys_map[i] == 0xFFFFFFFFFFFFFFFFL); i++);
        if(i == phys_size) return 0;      //Вся память занята

        //printf("alloc_pp 1");
	tmp = phys_map[i];
        //printf("tmp = %l\n", tmp);
	// Working aroung GCC 4.7 bug
#if 0
        for (j = 0; (j < 64)&&((tmp|(1<<j)) == tmp); j++);
        if (j == 64) return 0;   //А вдруг?
        phys_map[i] = (tmp|( 1<<j));
#else
	/* GCC версии 4.7 как-то странно обрабатывает 31-ый бит
	 * в 64-bit числе. При его установке старшие 32 бита
	 * также устанавливаются. Баги работы с sign 
	 * extension общие для всех компиляторов ветки 4.7.x
	 * Возможно, конкретно здесь баг проявляется из-за
	 * сборки на 32-битной машине.
	 */
	unsigned int *ph_map = (unsigned int *)phys_map;
	unsigned int itmp;
	for (k = 0; k < 2; k++)
	{
	  itmp = ph_map[i*2 + k];
	  
	  for (j = 0; (j < 32)&&((itmp|(1<<j)) == itmp); j++);
	  if (j != 32)
	    break;
	}
	if (j == 32)	// k можно не проверять
	  return 0;

        ph_map[i*2 + k] = (itmp|( 1<<j));
	j = j + k*32;
#endif
        //printf("Alloc p 0x%l\n", (64*i + j)<<12);
        //unsigned long *zero = (unsigned long *)(((64*i + j)<<12) + pool_addr);
        //for(k = 0; k<0x400; k++) zero[k] = 0x0000000000000000L;
        //printf("alloc_pp 3");
        return (void *)((64*i + j)<<12);
}

void free_phys_page(void *page)
{
        if(page == 0) return;
        unsigned long pageaddr = (unsigned long)page;
        pageaddr = pageaddr >> 12;
        //printf("1\n");
        phys_map[pageaddr>>6] = 
          phys_map[pageaddr>>6]&(~(1<<(pageaddr%64)));
}

void phys_init(unsigned long *last, unsigned long size,
		multiboot_info_t *mb)
{/*
	last_phys_page = *last;
        pool_addr = ((*last)&0xFFFFF000) + 0x1000;
        phys_map = (unsigned long *)(pool_addr);
        pool_size = pl_size - 0x1000;

        printf("pool_addr = 0x%X, size = %X\n", pool_addr, pool_size);

        clear_phys_map();*/

	/* На входе - размер ОЗУ в мегабайтах и первая свободная страница
	 * Собственно, одна страница хранит информацию о:
	 * 4096 байт/стр * 8 бит/байт = 32768 страницах, или 128 мегабайтах
	 */
	// Подключаем страницы из области битовой карты
	phys_map = (unsigned long *)(0xFFFFFFFFC0000000|*last);
	phys_size = size << 2;	// (size << 20) >> 18
	//BREAK();
	int sz;
	for (sz = 0; sz < size; sz += 128)
	{
		mount_page(*last, 0xFFFFFFFFC0000000|(*last));
		*last += 0x1000;
	}
	// Помечаем всю карту как занятую, за исключением явно указанных
	// в mmap свободных мест
	//BREAK();
        unsigned long i;
        for (i = 0; i < phys_size; i++) 
          phys_map[i] = 0xFFFFFFFFFFFFFFFFL;

	// Сверхопасный трюк - выдергивание себя самое за шнурки!
	mount_page(mb, 0xFFFFFFFFC0000000|(unsigned long)mb);
	mb = (multiboot_info_t *)(0xFFFFFFFFC0000000|(unsigned long)mb);
	mmap_info_t *mmap = (mmap_info_t *) (mb->mmap_addr);
	mount_page(mmap, 0xFFFFFFFFC0000000|(unsigned long)mmap);
	mmap = (mmap_info_t *)(0xFFFFFFFFC0000000|(unsigned long)mmap);

	//BREAK();
	for (;
              (unsigned long) mmap < 
		((0xFFFFFFFFC0000000|((unsigned long)mb->mmap_addr))
		 + mb->mmap_length);
              mmap = (mmap_info_t *) ((unsigned long) mmap
                       + mmap->size + sizeof (mmap->size)))
	  if (mmap->type == 1)
	  {
	     // Во втором мегабайте памяти лежит ядро;
	     // Вот и пусть себе лежит
	     if (mmap->base_addr == 0x100000)
		mmap->base_addr = (((*last)>>18) + 1) << 18;
/*
             printf ("base_addr = 0x%l,"
                     " length = 0x%l\n",
                     (unsigned long) mmap->base_addr,
                     (unsigned long) mmap->length);
*/
	     for (i = (mmap->base_addr >> 18);
		  i < ((mmap->base_addr + mmap->length) >> 18);
		  i++)
		phys_map[i] = 0L;
	  }
	umount_page(mb);
	umount_page(0xFFFFFFFFC0000000|(unsigned long)(mb->mmap_addr));
	printf("Init phys mem map complete - %dMB\n", size);
}
