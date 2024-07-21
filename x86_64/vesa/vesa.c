/*
 * vesa.c
 *
 * Простой видеодрайвер VESA-режима
 *
 */

#include <page.h>
#include <mem.h>
#include <vesa/vesa.h>
#include <vesa/font.h>
#include <string.h>

#include <debug.h>

static uint64_t vesa_width, vesa_height, vesa_bpp;
void test();
void putpixel(int x, int y);

void vesa_init(vesa_mode_info_t *vi)
{
  // BREAK();
  // Для начала отобразим буфер кадров полностью в пространство ядра
  void *ptr_log = VESA_FB_ADDR;
  void *ptr_phys = vi->lfb_ptr;
  uint64_t limit = (vi->h_res * vi->v_res * vi->bpp) >> 3;
  uint64_t i,j;

  for (i = 0; i < limit; i += 0x1000)
    mount_page_hw(ptr_phys + i, ptr_log + i);

  vesa_width = vi->h_res;
  vesa_height = vi->v_res;
  vesa_bpp = vi->bpp;

#if 0
  // Отлично, память примонтирована - теперь с ней можно работать
  short *v_mem = VESA_FB_ADDR;
  for (i = 0; i < limit>>1; i++)
    v_mem[i] = i;
#endif
  test();
}

void putpixel(int x, int y)
{
  uint8_t *fb8;
  uint16_t *fb16;
  uint32_t *fb32;

  fb8 = VESA_FB_ADDR;
  fb16 = VESA_FB_ADDR;
  fb32 = VESA_FB_ADDR;

  switch (vesa_bpp)
  {
    case 8:
      fb8[y*vesa_width + x] = 0xFF;
      break;

    case 16:
      fb16[y*vesa_width + x] = 0xFFFF;
      break;

    case 32:
      fb32[y*vesa_width + x] = 0xFFFFFFFF;
      break;

  }
}

// Перенесено из fbtest.c
uint8_t *utf2win(const uint8_t *utf, uint8_t *win)
{
  // Формат русских символов в UTF-8:
  // d0 90 - d0 bf : А-Я, а-п
  // d1 80 - d1 8f : р-я
  // d0 81 , d1 91 : Ё,ё
  // Формат русских символов в CP1251:
  // c0 - df : А-Я
  // e0 - ff : а-я
  // a8 , b8 : Ё,ё
  uint8_t *out = win;
  uint8_t bit;

  while(*utf)
  {
    if(*utf < 0x80) 
      *win++ = *utf++;
    else if (((*utf)&0xFE) == 0xd0)
    {
      bit = (*utf++)&1; 
      if (((*utf)&0x91) == ((0x81)|(bit<<4))) // Ёё
        *win++ = (*utf++) + 0x27;
      else
        *win++ = ((*utf++)|(bit<<6)) + 0x30;
    } else {
      // wrong symbol
      utf += 2;
      *win++ = '?';
    }
  }
  *win = 0;
  return out;
}

uint8_t font[256][8] = {0};

#define FB_WIDTH (400)
#define FB_HEIGHT (8)

uint8_t framebuf[FB_WIDTH][FB_HEIGHT] = {' '};
uint32_t x, y;

void put_char(uint8_t c)
{
  uint8_t i, j;
  for (i = 0; i < 8; i++)
  {
    for (j = 0; j < 8; j++)
      if ((1<<(7-j))&font[c][i])
        framebuf[x+j][y+i] = 'x';
      else
        framebuf[x+j][y+i] = ' ';
  }
  x += 8;
  if ((x+8) >= FB_WIDTH) // Следующий символ не поместится
  {
    x = 0;
    y += 8;
  }
}

void put_s(uint8_t *s)
{
  while(*s)
    put_char(*s++);
}

void load_font()
{
  uint32_t i,j;
  uint8_t bit;

  for (i = 0; i < 128; i++)
    for (j = 0; j < 128; j++)
    {
      bit = header_data[i*128 + j];
      if (bit)
      {
        font[(i>>3)*16 + (j>>3)][i%8] |= (1<<(7 - (j%8)));
      }
    }
}

void test()
{
  x = 0;
  y = 0;
  load_font();
  uint8_t *t = "Широкая электрификация южных губерний! Wild cat jumps over the lazy dog!";
  uint8_t *c = kmalloc(strlen(t)+1);
  utf2win(t, c);
  put_s(c);
#if 1
  for (y = 0; y < FB_HEIGHT; y++)
  {
    for (x = 0; x < FB_WIDTH; x++)
      if (framebuf[x][y] != ' ')
        putpixel(x, y);
  }
#endif
}

