#include <stdint.h>
#include <page.h>
#include <multiboot.h>
#include <intr.h>
#include <phys.h>
#include <mem.h>
#include <kprintf.h>

#include <debug.h>

#define TMP_MOUNT_ADDR 0xFFFFFFFFC0000000

pte_desc *PML4;    // PML4, основная таблица
pte_desc *kernel_pt;  // PT ядра

pte_desc create_page()
{
  pte_desc page;
  page.h = 0;
  //kprintf("create_page 1");
  page.p.address = ((uint64_t)alloc_phys_page())>>12;
  //kprintf("create_page 2");
  page.p.exists = 1;
  page.p.writable = 1;
  //kprintf("create_page = %l\n", page.h);
  return page;
}

pte_desc create_page_user()
{
  pte_desc page;
  page.h = 0;
  //kprintf("create_page 1");
  page.p.address = ((uint64_t)alloc_phys_page())>>12;
  page.p.r3_access = 1;
  //kprintf("create_page 2");
  page.p.exists = 1;
  page.p.writable = 1;
  //kprintf("create_page = %l\n", page.h);
  return page;
}

pte_desc calc_page(void *phys_addr)
{
  pte_desc page;
  page.h = 0;
  page.p.address = (uint64_t)phys_addr >> 12;
  page.p.exists = 1;
  page.p.writable = 1;
  return page;
}

pte_desc calc_page_hw(void *phys_addr)
{
  pte_desc page;
  page.h = 0;
  page.p.address = (uint64_t)phys_addr >> 12;
  page.p.exists = 1;
  page.p.writable = 1;
  page.p.cache_disable = 1;
  return page;
}

/*
 * TODO:
 * Какой-то старый и непонятный кусок кода.
 * Невооруженным взглядом видны ошибки.
 * Однако, желания его править сейчас нет.
 * Он не используется.
 */
#if 0
uint64_t find_free_page()
{       
  int i,j;
  // Ищем доступный каталог
  for(i = 0; i<0x1000; i++)
  {
    if(PML4[i].h == 0) break; // Если попавшийся каталог пуст, выйдем из цикла и создадим в нем хотя бы одну запись
    // Иначе ищем в нем свободные записи
    uint64_t *pde = (uint64_t *)(uint64_t)(PML4[i].p.address<<12);
    for(j = 0; j<0x1000; j++)
    {
      if(pde[j] == 0) return ((i<<22) + (j<<12));
    }
  }
  // Необходимо создать каталог
  PML4[i] = create_page();
  //kprintf("Need to create");
  return (i<<22);
}

void *alloc_page()
{
  uint64_t i,j,res;
  res = find_free_page();
  //kprintf("res = %X\n", res);
  j = res>>22;
  i = (res>>12)&0x3FF;
  pte_desc page = create_page();
  //kprintf("page = %X\n", page);
  pte_desc *tmp = (pte_desc *)(uint64_t)(PML4[j].p.address<<12);
  tmp[i] = page;
  return (void *)res;
}
  
void free_page(void *page)
{
  uint64_t i,j,phys;
  //kprintf("page = %X\n", page);
  i = ((uint64_t)page>>22);
  j = ((uint64_t)page>>12)&0x3FF;
  //kprintf("i = %X, j = %X\n",i,j);

  pte_desc addr = PML4[i];
  //kprintf("addr = %X\n",addr.h);

  pte_desc *p = (pte_desc *)(uint64_t)(addr.p.address<<12);
  phys = (p[j].p.address<<12);
  p[j].h = (uint64_t)0x00000000;
  //kprintf("phys = %X\n", phys);
  free_phys_page((void *)phys);
}
#endif // if 0

/*
 * Монтирует страницу на временный адрес
 */
void mount_page_t(void *phys_addr)
{
  intr_disable();
  //kprintf("Mount_t: %l\n", (uint64_t)phys_addr);
  kernel_pt[0] = calc_page(phys_addr);
  uint64_t inv_addr = TMP_MOUNT_ADDR; 
  asm("mov %0, %%rax\n \
       invlpg (%%rax)"::"m"(inv_addr));
  intr_enable();
}

void umount_page_t()
{
  kernel_pt[0].h = 0;
  uint64_t inv_addr = TMP_MOUNT_ADDR; 
  asm("mov %0, %%rax\n \
       invlpg (%%rax)"::"m"(inv_addr));
}
  
void mount_page_do(void *log_addr, pte_desc ph)
{
  //kprintf("Mounting 0x%X - 0x%l\n", phys_addr, log_addr);
  linear addr;
  addr.h = (uint64_t)log_addr;
  pte_desc volatile * volatile p = (pte_desc *)TMP_MOUNT_ADDR;

  mount_page_t((void *)PML4);
  //kprintf("PML4: %d\n", addr.l.pml4);
  pte_desc _p = p[addr.l.pml4];
  //kprintf("P is %l\n", _p.h);
  //BREAK();
  // Если необходимый каталог не существует
  if(_p.h == 0)
  {
    //kprintf("!\n");
    _p = create_page();
    _p.p.r3_access = ph.p.r3_access;
    p[addr.l.pml4] = _p;
  }
  umount_page_t();

  // Монтируем в пространство ядра PDP
  mount_page_t((void *)(_p.h&0xFFFFFFFFFFFFF000));
  _p = p[addr.l.pdp];
  if(_p.h == 0)
  { 
    //kprintf("! !\n");
    _p = create_page();
    _p.p.r3_access = ph.p.r3_access;
    p[addr.l.pdp] = _p;
  }
  umount_page_t();

  // PD
  mount_page_t((void *)(_p.h&0xFFFFFFFFFFFFF000));
  _p = p[addr.l.pd];
  if(_p.h == 0)
  { 
    //kprintf("! ! !\n");
    _p = create_page();
    _p.p.r3_access = ph.p.r3_access;
    p[addr.l.pd] = _p;
  }
  umount_page_t();

  // PT
  mount_page_t((void *)(_p.h&0xFFFFFFFFFFFFF000));
  p[addr.l.pt] = ph;

  umount_page_t();
}

void mount_page(void *phys, void *log)
{
  mount_page_do(log, calc_page(phys));
}

void mount_page_hw(void *phys, void *log)
{
  mount_page_do(log, calc_page_hw(phys));
}

void *umount_page(void *log_addr)
{
  //kprintf("Unmounting 0x%l...", log_addr);
  linear addr;
  addr.h = (uint64_t)log_addr;
  pte_desc volatile * volatile p = (pte_desc *)TMP_MOUNT_ADDR;

  mount_page_t((void *)PML4);
  //kprintf("PML4: %d\n", addr.l.pml4);
  pte_desc _p = p[addr.l.pml4];
  //kprintf("P is %l\n", _p.h);
  //BREAK();
  umount_page_t();
  // Если необходимый каталог не существует
  if(_p.h == 0)
  {
    return;
  }

  // Монтируем в пространство ядра PDP
  mount_page_t((void *)(_p.h&0xFFFFFFFFFFFFF000));
  _p = p[addr.l.pdp];
  umount_page_t();
  if(_p.h == 0)
  { 
    return;
  }

  // PD
  mount_page_t((void *)(_p.h&0xFFFFFFFFFFFFF000));
  _p = p[addr.l.pd];
  umount_page_t();
  if(_p.h == 0)
  { 
    return;
  }

  // PT
  mount_page_t((void *)(_p.h&0xFFFFFFFFFFFFF000));

  void *ret = p[addr.l.pt].h;
  p[addr.l.pt].h = p[addr.l.pt].h&0xFFFFFFFFFFFFFFFE;

  umount_page_t();
  //kprintf(" done\n");

  return ret;
}

void read_page(void *phys_addr, void *buf)
{
  volatile void *tmp = TMP_MOUNT_ADDR;

  mount_page_t(phys_addr);
  memcpy(buf, tmp, 4096);
  umount_page_t();
}

void write_page(void *phys_addr, void *buf)
{
  volatile void *tmp = TMP_MOUNT_ADDR;

  mount_page_t(phys_addr);
  memcpy(tmp, buf, 4096);
  umount_page_t();
}


/*
 * Копирует определенный каталог страниц
 * k - глубина вложенности:
 * 4 - pml4
 * 3 - pdp
 * 2 - pde
 * 1 - pte
 * 0 - копируем обычную страницу
 */
void *copy_page(void *phys_addr, int k)
{

//  kprintf("Remapping %l...\n", phys_addr);
  volatile pte_desc *buf = kmalloc(4096);
  uint64_t addr;
  read_page(phys_addr, buf);

  if (k > 0)
  {
    uint32_t i;
    for (i = 0; i < 512; i++)
    {
      addr = buf[i].p.address<<12;
      if (addr != 0x0000000000000000L)
      {
        /*
         * Тут мы используем неболшой хак
         * Дело в том, что нам известна только kernel_pte
         * Она идет нулевой записью в kernel_pde
         * Соответственно, если при попытке копирования
         * pde наталкиваемся на адрес kernel_pte, то это
         * pde ядра, и ее не нужно копировать - просто
         * возвращаем ее адрес обратно.
         * Условия в if расположены в порядке убывания
         * частоты срабатывания
         */
        //BREAK();
        if ((k == 1) && (i == 0) && 
            (addr == (((uint64_t)kernel_pt)&0x0FFFFFFF)))
          return phys_addr;
	else
        {
          buf[i].h = (buf[i].h&0x1F)
                     |(uint64_t)(copy_page(addr, k-1));
        }
      }
    }
  }

  void *p = alloc_phys_page();
  write_page(p, buf);
  kfree(buf);
  return p;
}

/*
 * Функция возвращает новый каталог страниц, аналогичный
 * уже имеющемуся. Страницы ядра подключаются как есть,
 * (просто записывается ссылка на PT ядра), остальные
 * имеющиеся - копируются.
 */
void *copy_pages()
{
  // Функция не оптимальна, но зато относительно проста

  uint64_t cr3;
  asm("mov %%cr3, %0\n":"=r"(cr3));
  return copy_page(cr3, 4);
}

/*
 * Функция переподключает физические страницы с одного логического адреса на другой
 */
void remap_pages(void *oldaddr, void *newaddr, uint64_t size)
{
  uint64_t i;
  void *tmp = 0;

  for (i = 0; i < size; i += 0x1000)
  {
    tmp = umount_page(oldaddr + i);
    mount_page(tmp, newaddr+i);
  }
}

/*
 * Функция выделяет блок из нескольких последовательных страниц, начиная с
 * некоторого адреса
 */
void alloc_pages(void *addr, uint64_t size)
{
  uint64_t i;
  for (i = 0; i < size; i += 0x1000)
  {
    mount_page_do(addr + i, create_page());
  }
}

void alloc_pages_user(void *addr, uint64_t size)
{
  uint64_t i;
  for (i = 0; i < size; i += 0x1000)
  {
    mount_page_do(addr + i, create_page_user());
  }
}

void page_init(uint64_t *last_phys_page)
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
  uint64_t *last = (uint64_t *)*last_phys_page;
  // Временно установим PT ядра на старый адрес
  kernel_pt = (void *)0xD000;
  PML4 = (void *)last;
  
  /* PML4 */
  zeromem(last, 4096);
  last[511] = (calc_page(last + 0x200)).h;
  //kprintf("1 = %l\n", last[511]);
  last += 0x200;  // 0x1000 / sizeof(uint64_t)
  /* PDP */
  zeromem(last, 4096);
  last[511] = (calc_page(last + 0x200)).h;
  //kprintf("2 = %l\n", last[511]);
  last += 0x200;
  /* PD */
  zeromem(last, 4096);
  last[0] = (calc_page(last + 0x200)).h;
  //kprintf("3 = %l\n", last[0]);
  last += 0x200;
  //BREAK();
  /* PT */
  zeromem(last, 4096);
  uint64_t i;
  for(i = 0xB8000; i <= (uint64_t)last; i += 0x1000)
  {
  //  mount_page(i, 0xFFFFFFFFC0000000+i);
    last[i>>12] = (calc_page((void *)i)).h;
  }

  // Установим новую Kernel PT
  kernel_pt = (pte_desc *)(((uint64_t)last) | 0xFFFFFFFFC0000000);
  last += 0x200;

  intr_disable();  //Отключим прерывания
  //BREAK();
  asm("  movq $0x000ffffffffff000, %%rax \n \
         andq %%rax, %0 \n \
         mov %0, %%cr3"
  ::"r"((uint64_t)PML4):"rax");  //Загрузим PML4
  //BREAK();
  intr_enable();

  *last_phys_page = (uint64_t)last;

  kprintf("Pagination enabled!\n");
}

