#include <ktty.h>
#include <stdint.h>

#define SCREEN_HEIGHT  25
#define SCREEN_WIDTH  80
//#define SCREEN_START_COLOUR  0xB8000  // Начало видеопамяти для цветных видеоадаптеров
#define SCREEN_START_COLOUR 0xFFFFFFFFC00B8000
#define SCREEN_START_MONO  0xB0000 //Начало видеопамяти для монохромных адаптеров
#define SCREEN_SIZE  (SCREEN_HEIGHT*SCREEN_WIDTH)

static uint64_t screen_base = 0;
static uint64_t screen_pos = 0;
static uint8_t ktty_attribute = 0x07;

/* 
 * Процедура инициализации 
 */
void ktty_init(void)
{
  // Определяем тип видео
  //char c = (*(uint16_t *)0x410)&0x30;
  /*if(c == 0x30) //Монохромный
    screen_base = SCREEN_START_MONO;
  else*/  //Цветной
    screen_base = SCREEN_START_COLOUR;
  ktty_attribute = 0x07;
  ktty_clear();
}

/* 
 * Очистка экрана 
 */
void ktty_clear(void)
{
  long i;
  for(i = 0; i < SCREEN_SIZE*2; i++)
    *((uint8_t *)(screen_base + i)) = 0x0;

  screen_pos = 0;
}

/*
 * Сдвиг экрана вверх 
 */
void ktty_shift(void)
{
  long i;
  for(i = 0; i < (SCREEN_SIZE - SCREEN_WIDTH)*2; i++)
    *((uint8_t *)(screen_base + i)) = *((uint8_t *)(screen_base + i + SCREEN_WIDTH*2));

  for(; i < SCREEN_SIZE*2; i++)
    *((uint8_t *)(screen_base + i)) = 0x0;

  screen_pos = SCREEN_SIZE - SCREEN_WIDTH;
}

void ktty_putc(char c)
{
  switch (c) {
    case '\n':
      screen_pos += SCREEN_WIDTH;
      screen_pos -= screen_pos%SCREEN_WIDTH;
      break;
    default:
      *((uint8_t *)(screen_base + screen_pos*2)) = c;
      *((uint8_t *)(screen_base + screen_pos*2 + 1)) = ktty_attribute;
      screen_pos++;
      break;
  }
  
  if(screen_pos >= SCREEN_SIZE) ktty_shift();
}

void ktty_puts(char *s)
{
  while(*s)
  {
    ktty_putc(*s);
    *s++;
  }
}

