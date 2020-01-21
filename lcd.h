/*
 * lcd.h
 *
 *  Created on: 15 sty 2020
 *      Author: Badziew
 */

#ifndef LCD_H_
#define LCD_H_

#include "bool.h"
#include <stdint.h>

#define CMD FALSE
#define DATA TRUE

void lcd_send(uint8_t, BOOL);
void lcd_cls(void);
void lcd_string(const char*);
void lcd_num(uint8_t);
void lcd_gotoxy(uint8_t, uint8_t);
void lcd_init(void);
void lcd_load_chars(void);
void lcd_print_bin(uint8_t, uint8_t);

#endif /* LCD_H_ */
