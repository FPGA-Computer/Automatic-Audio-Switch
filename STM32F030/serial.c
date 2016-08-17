/*
 * serial.c
 *
 * Created: March-09-16, 8:48:47 PM
 *  Author: K. C. Lee
 * Copyright (c) 2015 by K. C. Lee
 
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

#include "hardware.h"
#include "fifo.h"

FIFO_DECL(RxBuf,RX_FIFO_Size);
FIFO_DECL(TxBuf,TX_FIFO_Size);

void USART_Init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;									// Enable UART1 clock
  USART1->BRR = CPU_CLOCK/UART_BAUD;										// Set default baud rate

  USART1->CR1 = USART_CR1_RXNEIE|												// Receive not empty
								USART_CR1_RE|USART_CR1_TE|USART_CR1_UE; // 8-bit, 1 start bit, no parity, 16X oversampling

	FIFO_Clear((FIFO*)RxBuf);
	FIFO_Clear((FIFO*)TxBuf);
	
  // NVIC IRQ
  NVIC_SetPriority(USART1_IRQn,USART_IRQ_PRIORITY);			// Lowest priority																	// Highest priority
  NVIC_EnableIRQ(USART1_IRQn);
}

void USART1_IRQHandler(void)
{
	if(USART1->ISR & USART_ISR_RXNE)											// Rx data
	{
		FIFO_Write((FIFO*)RxBuf,USART1->RDR);	
	}
	
	if(USART1->ISR & USART_ISR_TXE)												// Tx empty
	{
		if(!FIFO_Read((FIFO*)TxBuf,(uint8_t *)&USART1->TDR))// Little endian
			USART1->CR1 &= ~USART_CR1_TXEIE;									// Disable Tx Empty interrupt
	}
}

void USART_PutChar( uint8_t data )
{
	while(!FIFO_Write((FIFO*)TxBuf,data))
	  /* busy wait */;
	
  USART1->CR1 |= USART_CR1_TXEIE;												// Enable Tx Empty interrupt
}
