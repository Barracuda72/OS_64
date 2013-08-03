#include <page.h>
#include <multiboot.h>
#include <intr.h>
#include <phys.h>
#include <klibc.h>

#include <debug.h>

#define TMP_MOUNT_ADDR 0xFFFFFFFFC0000000

pte_desc *PML4;		// PML4, основная таблица
pte_desc *kernel_pt;	// PT ядра

pte_desc create_page()
{
	pte_desc page;
	page.h = 0;
	//printf("create_page 1");
	page.p.address = ((unsigned long)alloc_phys_page())>>12;
	//printf("create_page 2");
	page.p.exists = 1;
	page.p.writable = 1;
	printf("create_page = %l\n", page.h);
	return page;
}

pte_desc calc_page(void *phys_addr)
{
	pte_desc page;
	page.h = 0;
	page.p.address = (unsigned long)phys_addr >> 12;
	page.p.exists = 1;
	page.p.writable = 1;
	return page;
}

unsigned long find_free_page()
{       
	int i,j;
	//Ищем доступный каталог
        for(i = 0; i<0x1000; i++)
        {
                if(PML4[i].h == 0) break; //Если попавшийся каталог пуст, выйдем из цикла и создадим в нем хотя бы одну запись
		//Иначе ищем в нем свободные записи
                unsigned long *pde = (unsigned long *)(unsigned long)(PML4[i].p.address<<12);
                for(j = 0; j<0x1000; j++)
                {
                        if(pde[j] == 0) return ((i<<22) + (j<<12));
                }
        }
	//Необходимо создать каталог
	PML4[i] = create_page();
	//printf("Need to create");
	return (i<<22);
}

void *alloc_page()
{
	unsigned long i,j,res;
	res = find_free_page();
	//printf("res = %X\n", res);
	j = res>>22;
	i = (res>>12)&0x3FF;
	pte_desc page = create_page();
	//printf("page = %X\n", page);
	pte_desc *tmp = (pte_desc *)(unsigned long)(PML4[j].p.address<<12);
	tmp[i] = page;
	return (void *)res;
}
	
void free_page(void *page)
{
	unsigned long i,j,phys;
	//printf("page = %X\n", page);
	i = ((unsigned long)page>>22);
	j = ((unsigned long)page>>12)&0x3FF;
	//printf("i = %X, j = %X\n",i,j);

	pte_desc addr = PML4[i];
	//printf("addr = %X\n",addr.h);

	pte_desc *p = (pte_desc *)(unsigned long)(addr.p.address<<12);
	phys = (p[j].p.address<<12);
	p[j].h = (unsigned long)0x00000000;
	//printf("phys = %X\n", phys);
	free_phys_page((void *)phys);
}

void mount_page_t(void *phys_addr)
{
	//printf("Mount_t: %l\n", (unsigned long)phys_addr);
	// Монтирует страницу на временный адрес
	kernel_pt[0] = calc_page(phys_addr);
	unsigned long inv_addr = TMP_MOUNT_ADDR; 
	asm("mov %0, %%rax\n \
	     invlpg (%%rax)"::"m"(inv_addr));
	//asm("mov %cr3, %rax\n mov %rax, %cr3");	//HACK
}

void umount_page_t()
{
	kernel_pt[0].h = 0;
	unsigned long inv_addr = TMP_MOUNT_ADDR; 
	asm("mov %0, %%rax\n \
	     invlpg (%%rax)"::"m"(inv_addr));
	//asm("mov %cr3, %rax\n mov %rax, %cr3");	//HACK
}
	
void mount_page(void *phys_addr, void *log_addr)
{
	printf("Mounting 0x%X - 0x%l\n", phys_addr, log_addr);
	linear addr;
	addr.h = (unsigned long)log_addr;
	pte_desc volatile * volatile p = (pte_desc *)TMP_MOUNT_ADDR;

	mount_page_t((void *)PML4);
	//printf("PML4: %d\n", addr.l.pml4);
	pte_desc _p = p[addr.l.pml4];
	//printf("P is %l\n", _p.h);
	BREAK();
	// Если необходимый каталог не существует
	if(_p.h == 0)
	{
		printf("!\n");
		_p = create_page();
		p[addr.l.pml4] = _p;
	}
	umount_page_t();

	// Монтируем в пространство ядра PDP
	mount_page_t((void *)(_p.h&0xFFFFFFFFFFFFF000));
	_p = p[addr.l.pdp];
	if(_p.h == 0)
	{ 
		printf("! !\n");
		_p = create_page();
		p[addr.l.pdp] = _p;
	}
	umount_page_t();

	// PD
	mount_page_t((void *)(_p.h&0xFFFFFFFFFFFFF000));
	_p = p[addr.l.pd];
	if(_p.h == 0)
	{ 
		printf("! ! !\n");
		_p = create_page();
		p[addr.l.pd] = _p;
	}
	umount_page_t();

	// PT
	mount_page_t((void *)(_p.h&0xFFFFFFFFFFFFF000));
	p[addr.l.pt] = calc_page(phys_addr);

	umount_page_t();

}

void page_init(unsigned long *last_phys_page)
{
	/*
		!!!
		Мы можем использовать last_phys_page как указатель,
		поскольку пока включена и идентичная адресация также
		!!!
	*/

	/*
		CAUTION!
		Pointer ariphmetic!
		last_phys_page == address of variable that stores address of last phys page
		last == address of last phys page
		!!!
	*/
        unsigned long *last = (unsigned long *)*last_phys_page;
	// Временно установим PT ядра на старый адрес
	kernel_pt = (void *)0xD000;
	PML4 = (void *)last;
	
	/* PML4 */
	zeromem(last, 4096);
	last[511] = (calc_page(last + 0x200)).h;
	//printf("1 = %l\n", last[511]);
	last += 0x200;	// 0x1000 / sizeof(unsigned long)
	/* PDP */
	zeromem(last, 4096);
	last[511] = (calc_page(last + 0x200)).h;
	//printf("2 = %l\n", last[511]);
	last += 0x200;
	/* PD */
	zeromem(last, 4096);
	last[0] = (calc_page(last + 0x200)).h;
	//printf("3 = %l\n", last[0]);
	last += 0x200;
	//BREAK();
	/* PT */
	zeromem(last, 4096);
	unsigned long i;
	for(i = 0xB8000; i <= (unsigned long)last; i += 0x1000)
	{
	//	mount_page(i, 0xFFFFFFFFC0000000+i);
		last[i>>12] = (calc_page((void *)i)).h;
	}

	// Установим новую Kernel PT
	kernel_pt = (pte_desc *)(((unsigned long)last) | 0xFFFFFFFFC0000000);
	last += 0x200;

	intr_disable();	//Отключим прерывания
	//BREAK();
	asm("	movq $0x000ffffffffff000, %%rax \n \
		andq %%rax, %0 \n \
		mov %0, %%cr3"
	::"r"((unsigned long)PML4):"rax");	//Загрузим PML4
	//BREAK();
	intr_enable();

	*last_phys_page = (unsigned long)last;

	printf("Pagination enabled!\n");
}

