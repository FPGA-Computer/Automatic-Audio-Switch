/*
 * lib.h
 *
 * Created: 08/09/2012 10:19:34 PM
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

#ifndef LIB_H_
#define LIB_H_

#include <inttypes.h>
#include "serial.h"

#define LeadingZero			0x01
#define RightJustify		0x02
#define LeftJustify			0x04

uint8_t Puts(const char *s);
void NewLine(void);
void Space(void);
void FillSpc(char n);
void Puthex2(uint8_t n);
void Puthex4(uint16_t n);
void Puthex8(uint32_t n);
uint8_t Hex(char ch);

char ToUpper(char ch);
char IsWhiteSpc(char ch);
void Print_Uint(uint16_t Num, uint16_t Div, uint8_t Format);
#endif /* LIB_H_ */

