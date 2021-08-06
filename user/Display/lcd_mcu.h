#ifndef _LCD_MCU_H
#define _LCD_MCU_H

#include <unistd.h>
static void mcu_lcd_clear(uint16_t color);
static int mcu_lcd_init(lcd_para_t *lcd_para);
#endif
