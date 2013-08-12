#ifndef __KBD_H__
#define __KBD_H__

#include <intr.h>
#include <ioport.h>
#include <ktty.h>

/*
 * Обработчик прерывания клавиатуры
 */
IRQ_HANDLER(kbd_intr)
{
  unsigned char scancode;
  //unsigned char ascii;
  unsigned char creg;

  // Прочитаем скан-код из порта 0x60
  scancode = inb(0x60);
  
   switch(scancode) {
    
  // Скан-коды нажатого шифта
  /*
  case 0x36: 
  case 0x2A:
    shift = 1;
    break;

  // Скан-коды отпущенного шифта
  case 0x36 + 0x80:
  case 0x2A + 0x80:
    shift = 0;
        break;
    */
  default:

    // Если клавиша отпущена...
  /*
    if(scancode >= 0x80) {
      //То ничего не делать      
    } else {
      // А если нажата...
  
      if(shift) {
        // Если шифт нажат, то преобразовать скан-код в "шифтнутое" ASCII
        ascii = scancodes_shifted[scancode];

      } else {
        //А если не нажат - то в обычное
        ascii = scancodes[scancode];
      }
      
      //Если в результате преобразования нажата клавиша с каким-либо
      //символом, то вывести его на экран
      if(ascii != 0) {
        putchar(ascii);
      }
    }
    */
    ktty_putc(scancode);
    break;
  }

  // Считаем байт состояния клавиатуры
  creg = inb(0x61);
  
  // Установим в нем старший бит
  creg |= 1;

  // И запишем обратно
  outb(0x61, creg);
}

#endif //__KBD_H__

