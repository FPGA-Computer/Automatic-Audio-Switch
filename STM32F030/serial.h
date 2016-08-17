/*
 * serial.h
 *
 * Created: March-09-16, 8:49:01 PM
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
 
#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <stdio.h>
#include <stdint.h>
#include "stm32f0xx.h"
#include "system_stm32f0xx.h"
#include "fifo.h"

#ifndef UART_BAUD
#define UART_BAUD								115200
#endif

/* Buffer in 2^n */
#define RX_FIFO_Size						8
#define TX_FIFO_Size						8
#define USART_IRQ_PRIORITY  		3

void USART_Init(void);
void USART_PutChar( uint8_t data );

extern FIFO_Data_t RxBuf[], TxBuf[];
#endif
