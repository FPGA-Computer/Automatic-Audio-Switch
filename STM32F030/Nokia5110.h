/*
 * Nokia5110.h
 *
 * Created: 09/07/2013 1:45:15 PM
 *  Author: K. C. Lee
 * Copyright (c) 2016 by K. C. Lee
 
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.

	If not, see http://www.gnu.org/licenses/gpl-3.0.en.html
 */ 

#ifndef NOKIA5110_H_
#define NOKIA5110_H_

#define SPI_OVERCLOCK

#ifdef SPI_OVERCLOCK
//SCK=48MHz/8 = 6MHz
#define SPI_BITRATE				SPI_CR1_BR_1
#else
// SCK=48MHz/16 = 3MHz
#define SPI_BITRATE				(SPI_CR1_BR_1|SPI_CR1_BR_0)
#endif

#define LCD_DMA_IRQ_PRIORITY	2

#define LCD_MAX_X					84
#define LCD_MAX_Y					6
#define LCD_TEXT_WIDTH		5
#define LCD_SYM_WIDTH			(LCD_TEXT_WIDTH+1)
#define LCD_MAXCOL				(LCD_MAX_X/LCD_TEXT_WIDTH)
#define LCD_MAXROW				LCD_MAX_Y
#define LCD_PIX_PER_ROW		8

#include "hardware.h"

#define LCD_Home()				LCD_Moveto(0,0)

enum LCD_Cmd
{
	LCD_CMD_NOP	= 0x00, LCD_CMD_FH0 = 0x20,
	LCD_CMD_FH1 = 0x21, LCD_Disp_Off = 0x08,
	LCD_Disp_On	= 0x0c,	LCD_Addr_Y = 0x40,
	LCD_Addr_X = 0x80,	LCD_TempCtrl = 0x04,
	LCD_SetBias	=	0x13,	LCD_Contrast = 0x80,
	LCD_ContrastDefault = 0x40
};

extern uint8_t TextAttr;
extern uint8_t CurCol, CurRow;
extern const uint8_t LCD_InitCmd[];
extern const uint8_t LCD_Char8x5[];

void SPI_ByteWrite(uint8_t byte);
void SPI_Block_Write(const uint8_t *ptr, uint16_t size);
void SPI_Block_Fill(uint8_t Fill, uint16_t size);
void LCD_Init(void);
void LCD_CORD_XY(uint8_t X, uint8_t Y);
void LCD_Moveto(uint8_t TextCur_X, uint8_t TextCur_Y);
void Cursor_NewLine(void);
void LCD_Cls(void);
void LCD_PutCh(uint8_t Ch);
void LCD_Puts(const char *str);
void LCD_HalfWidthCh(uint8_t Ch);

#define TextAttr_Normal			0x00
#define TextAttr_Invert			0x01
#define TextAttr_Large			0x02

#define LCD_CmdMode()				GPIOA->BSRR = PIN_CLR(LCD_DC)
#define LCD_DataMode()			GPIOA->BSRR = PIN_SET(LCD_DC)

// control/special characters

#define Ch_NewLine					'\n'

// low priority
#define SPI_DMA_PRIORITY    0
#endif /* NOKIA5110_H_ */
