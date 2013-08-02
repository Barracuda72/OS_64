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
unsigned long phys_mem_map_end;

unsigned long pool_addr;
unsigned long pool_size;

unsigned long last_phys_page;	// Последняя физическая страница

void *alloc_phys_page()
{
        unsigned long i,j,k;
        for (i = 0; (i < (pool_size>>17))&&(phys_map[i] == 0xFFFFFFFFL); i++);
        if(i == (pool_size>>17)) return 0;      //Вся память занята

        unsigned long tmp = phys_map[i];
        //printf("tmp = %X\n", tmp);
        //printf("alloc_pp 1");
        for(j = 0; (j < 32)&&((tmp|(1<<j)) == tmp); j++);
        if(j == 32) return 0;   //А вдруг?
        phys_map[i] = (tmp|( 1<<j));
        //printf("pool %X, allocated page 0x%X\n", pool_addr, (32*i + j)<<12);
        //unsigned long *zero = (unsigned long *)(((32*i + j)<<12) + pool_addr);
        //for(k = 0; k<0x400; k++) zero[k] = 0x00000000L;
        //printf("alloc_pp 3");
        return (void *)(((32*i + j)<<12) + pool_addr);
}

void free_phys_page(void *page)
{
        if(page == 0) return;
        unsigned long pageaddr = (unsigned long)page;
        pageaddr -= pool_addr;
        pageaddr = pageaddr >> 12;
//      printf("1\n");
        phys_map[pageaddr>>5] = phys_map[pageaddr>>5]&(0xFFFFFFFF - (1<<(pageaddr%32)));
}

void clear_phys_map()
{
        unsigned long i;
        phys_map = (unsigned long *)pool_addr;
        for (i = 0; i < (pool_size>>17); i++) phys_map[i] = 0x00000000L;        //Очищаем память
        //Помечаем область самой карты как занятую
        for(i = 0; i < (pool_size>>12); i++) phys_map[(i>>5)] |= ((1<<i)%32);
}

void phys_init(unsigned long *last, unsigned long size)
{/*
	last_phys_page = *last;
        pool_addr = ((*last)&0xFFFFF000) + 0x1000;
        phys_map = (unsigned long *)(pool_addr);
        pool_size = pl_size - 0x1000;

        printf("pool_addr = 0x%X, size = %X\n", pool_addr, pool_size);

        clear_phys_map();*/
}
