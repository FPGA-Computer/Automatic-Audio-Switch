/*
 * fifo.h
 *
 * Created: March-11-16, 12:03:08 AM
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

#ifndef _FIFO_H_
#define _FIFO_H_

#include <stdint.h>

typedef uint8_t FIFO_Data_t;
typedef uint8_t FIFO_Index_t;

typedef struct
{
	FIFO_Index_t	SizeMask;	
	volatile FIFO_Index_t Head;
	volatile FIFO_Index_t Tail;
} FIFO;

#define FIFO_DECL(Name,Size)	FIFO_Data_t Name[Size+sizeof(FIFO)] = { Size-1 }
#define FIFO_BUF(X)						((FIFO_Data_t *)&X[1])
#define FIFO_INC(INDEX,MOD1)	((INDEX+1)&(MOD1))

void    FIFO_Clear(FIFO *Queue);
uint8_t FIFO_Write(FIFO *Queue, FIFO_Data_t data);
uint8_t FIFO_WriteAvail(FIFO *Queue);
uint8_t FIFO_Read(FIFO *Queue, FIFO_Data_t *data);
uint8_t FIFO_ReadAvail(FIFO *Queue);
uint8_t Getc(FIFO *Queue);
uint8_t Getc_Timeout(FIFO *Queue, FIFO_Data_t *data, uint16_t Timeout);

#define TIME_INFINITE 0

#endif
