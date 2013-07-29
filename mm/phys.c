#include <phys.h>
#include <page.h>

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

void mem_init(unsigned long pl_addr, unsigned long pl_size)
{
	last_phys_page = (pl_addr&0x0FFFF000) + 0x1000;
	printf("Last phys page is %l\n", last_phys_page);
	page_init(&last_phys_page);
        //pool_addr = (pl_addr>>12)<<12 + 0x1000;
        //phys_map = (unsigned long *)(pool_addr);
        //pool_size = pl_size - 0x1000;

        //printf("pool_addr = 0x%X, size = %X\n", pool_addr, pool_size);

        //clear_phys_map();
}
