/*
 * fifo.c
 *
 * Created: March-16-2016, 3:52:04 PM
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
 
#include "fifo.h"

void FIFO_Clear(FIFO *Queue)
{
	Queue->Head = Queue->Tail = 0;
}

uint8_t FIFO_WriteAvail(FIFO *Queue)
{
  return(FIFO_INC(Queue->Head,Queue->SizeMask)!=Queue->Tail);
}

uint8_t FIFO_Write(FIFO *Queue, FIFO_Data_t data)
{
	if(FIFO_WriteAvail(Queue))
	{
		Queue->Head = FIFO_INC(Queue->Head,Queue->SizeMask);
		FIFO_BUF(Queue)[Queue->Head] = data;
		return 1;	
	}
	return 0;
}

uint8_t FIFO_ReadAvail(FIFO *Queue)
{
	return(Queue->Head != Queue->Tail);
}

uint8_t FIFO_Read(FIFO *Queue, FIFO_Data_t *data)
{
	if (FIFO_ReadAvail(Queue))
	{
		Queue->Tail = FIFO_INC(Queue->Tail,Queue->SizeMask);	
		*data = FIFO_BUF(Queue)[Queue->Tail];
		return 1;
	}
	return 0;	
}

uint8_t Getc(FIFO *Queue)
{
	uint8_t ch;
	
	while(!FIFO_Read(Queue,&ch))
	  /* busy wait */;
	
	return ch;
}

