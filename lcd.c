/*
 * lcd.c
 *
 *  Created on: 15 sty 2020
 *      Author: Badziew
 */
#include "lcd.h"
#include "bool.h"
#include <avr/io.h>
#include <util/delay.h>

#define RS (1<<PD2)
#define E (1<<PD3)
#define LCD_DDR DDRD

void lcd_send(uint8_t byte, BOOL cmd_data)
{
	if(!cmd_data)
		PORTD &= ~RS;
	else
		PORTD |= RS;
	PORTD = (PORTD & 0x0F) | (byte & 0xF0);
	PORTD |= E;
	_delay_us(1);
	PORTD &= ~E;
	PORTD= (PORTD & 0x0F) | (byte << 4);
	PORTD |= E;
	_delay_us(1);
	PORTD &= ~E;
	_delay_us(100);
}
void lcd_cls(void)
{
	lcd_send(0x01,CMD);
	_delay_ms(1);
	lcd_send(0x80,CMD);
	_delay_ms(1);
}
void lcd_string(const char *str)
{
	while(*str)
		lcd_send(*str++,DATA);
}
void lcd_num(uint8_t num)
{
	lcd_send((num/100)+0x30,DATA);
	lcd_send(((num/10)%10)+0x30,DATA);
	lcd_send((num%10)+0x30,DATA);
}
void lcd_gotoxy(uint8_t x, uint8_t y)
{
	if(x==1)
		lcd_send(0x80+y-1,CMD);
	else if(x==2)
		lcd_send(0xC0+y-1,CMD);
}
void lcd_init(void)
{
	LCD_DDR = 0xFF;
	_delay_ms(20);
	lcd_send(0x30,CMD); //WTF?! Weird 4-bit init conditions...
	_delay_ms(5);
	lcd_send(0x30,CMD);
	_delay_ms(1);
	lcd_send(0x30,CMD);
	_delay_ms(1);
	lcd_send(0x02,CMD);
	lcd_send(0x28,CMD); //2 lines, 5*8 matrix, 4-bit
	lcd_send(0x0C,CMD); //Display on cursor off
	lcd_send(0x06,CMD); //Increase cursor position, scroll off
	lcd_cls();
}

void lcd_load_chars(void)
{
	#define NUM_OF_CHARS 7
	#define NUM_OF_ROWS 8
	uint8_t i, j;

	lcd_send(0x40,CMD); //set pointer to first CGRAM address
	for(i = 0; i < NUM_OF_CHARS; i++)
	{
		for(j = 0; j < (NUM_OF_ROWS - i - 1); j++)
		{
			lcd_send(0b00000,DATA);
		}
		for(j = 0; j < (i + 1); j++)
		{
			lcd_send(0b11111,DATA);
		}
	}
}

void lcd_print_bin(uint8_t bin, uint8_t mag) //display magnitude pseudo-logarithmically
{
	if(mag > 40)
	{
		mag = 40;
	}
	if(mag < 8)
	{
		lcd_gotoxy(1, bin); //top line
		lcd_send(0x20, DATA); //clear
		lcd_gotoxy(2, bin); //bottom line
		if(mag == 0 || mag == 1)
		{
			lcd_send(0x00, DATA); //display either one bar on the bottom
		}
		else
		{
			lcd_send(mag - 1, DATA); //or linearly higher bin
		}
	}
	else
	{
		lcd_gotoxy(2, bin); //bottom line
		lcd_send(0xFF, DATA); //fill
		lcd_gotoxy(1, bin); //top line
		mag -= 8; //subtract the value already displayed on bottom line
		mag /= 4; //scale the remainder for "pseudo-logaritmicity"
		if(mag > 0)
		{
			if(mag >= 8)
			{
				lcd_send(0xFF, DATA);
			}
			else
			{
				lcd_send(mag - 1, DATA); //display
			}
		}
	}
}

//void lcd_print_bin(uint8_t bin, uint8_t mag) //display magnitude pseudo-logarithmically
//{
//	if(mag > 40)
//		mag = 40;
//	if(mag > 8) //fill the bottom and prepare to display on top
//	{
//		lcd_gotoxy(2, bin);
//		lcd_send(0xFF, DATA);
//		mag -= 8;
//		mag /= 4;
//		lcd_gotoxy(1, bin);
//	}
//	else //clear the top and prepare to display on bottom
//	{
//		lcd_gotoxy(1, bin);
//		lcd_send(0x20, DATA);
//		lcd_gotoxy(2, bin);
//	}
//
//	if(mag == 0)
//	{
//		lcd_send(0x20, DATA);
//	}
//	else if(mag == 8)
//	{
//		lcd_send(0xFF, DATA);
//	}
//	else
//	{
//		lcd_send(mag - 1, DATA);
//	}
//}
