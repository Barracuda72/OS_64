#include <phys.h>
#include <page.h>

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
        unsigned long i,j,k;
	// Начинаем поиски с первого мегабайта
        for (i = (1<<2); (i < phys_size) 
                    &&(phys_map[i] == 0xFFFFFFFFFFFFFFFFL); i++);
        if(i == phys_size) return 0;      //Вся память занята

        unsigned long tmp = phys_map[i];
        //printf("tmp = %X\n", tmp);
        //printf("alloc_pp 1");
        for(j = 0; (j < 64)&&((tmp|(1<<j)) == tmp); j++);
        if(j == 64) return 0;   //А вдруг?
        phys_map[i] = (tmp|( 1<<j));
        printf("Allocated page 0x%l\n", (64*i + j)<<12);
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
        printf("1\n");
        phys_map[pageaddr>>6] = 
          phys_map[pageaddr>>6]&(~(1<<(pageaddr%64)));
}

void phys_init(unsigned long *last, unsigned long size)
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
	printf("Init phys mem map - %dMB\n", size);
	phys_map = (unsigned long *)(0xFFFFFFFFC0000000|*last);
	phys_size = size << 2;	// (size << 20) >> 18
	for (; size > 0; size -= 128)
	{
		mount_page(*last, 0xFFFFFFFFC0000000|(*last));
		last += 0x200;
	}
	// Очищаем карту	
        unsigned long i;
        for (i = 0; i < phys_size; i++) 
          phys_map[i] = 0x0000000000000000L;

	// Помечаем область 0x100000 - 0x140000 как занятую (позже сделаю нормально,
	// пока главное отловить баг
	phys_map[1<<2] = 0xFFFFFFFFFFFFFFFFL;
}
