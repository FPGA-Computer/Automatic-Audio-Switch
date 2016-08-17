/*
 * IR Remote.c
 *
 * Created: June-18-16, 6:04:57 PM
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
 
#include "IR Remote.h"
 
IR_Data IR_State;
FIFO_DECL(IR_Buf,IR_FIFO_Size);

void IR_Init(void)
{	
	IR_State.Bit = 0;
	FIFO_Clear((FIFO*)IR_Buf);
	
	// Enable TIM14
	RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;

	// filter clock = /4, Timer clock = 48MHz/12 = 4MHz -> overflows at 16.384ms
  TIM14->CR1 |= TIM_CR1_CKD_1|TIM_CR1_CEN|TIM_CR1_URS;		
	TIM14->PSC = TIM14_PRESCALER - 1;
	
	// IRQ: Input Capture 1
	TIM14->DIER |= TIM_DIER_CC1IE|TIM_DIER_UIE;	
	
	// Input capture 1, filter: N=8, freq = 48MHz/4/32 (stable for 21us/46.8kHz)
	TIM14->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_IC2F_0 | TIM_CCMR1_IC2F_1|
									TIM_CCMR1_IC2F_2 | TIM_CCMR1_IC2F_3;
	
	// Input capture 1: both rising and falling edges
	TIM14->CCER = TIM_CCER_CC1E|TIM_CCER_CC1P|TIM_CCER_CC1NP;
	
	IR_State.Bit=IR_Idle;
	
  // NVIC IRQ
  NVIC_SetPriority(TIM14_IRQn,IR_IRQ_PRIORITY);		// Lowest priority
  NVIC_EnableIRQ(TIM14_IRQn);
}

void TIM14_IRQHandler(void)
{ 
	uint16_t delta;
	
	if(TIM14->SR & TIM_SR_CC1IF)
	{
		TIM14->SR &= ~TIM_SR_CC1IF;									// Clear Input Capture IRQ
		
		delta = TIM14->CNT;
		TIM14->CNT = 0;
				
		if (!IR_INP)																// falling edge
		{	
			if(IR_State.Bit == IR_Idle)	
				IR_State.Bit = IR_Start;						

			// 4.5ms space
			else if(IR_PULSE(IR_NEC_START,delta)&&
				 (IR_State.Bit == IR_Lead))
			{		
				IR_State.Bit = IR_Byte0_0;
				IR_State.Word = 0;
			}
			// 562.5us pulse -> '0'
			else if(IR_PULSE(IR_NEC_BIT0,delta))
			{
				IR_State.Scratch = IR_State.Scratch>>1;			
				IR_State.Bit++;			
			}
			// 1.6875ms space -> '1'
			else if(IR_PULSE(IR_NEC_BIT1,delta))
			{
				IR_State.Scratch = (IR_State.Scratch>>1)|0x80;			
				IR_State.Bit++;					
			}				
			else // garbage
				IR_State.Bit = IR_Idle;		
		}
		else																					// rising edge
		{
			// 9ms 
			if(IR_PULSE(IR_NEC_PRE,delta))
				IR_State.Bit = IR_Lead;
			
			// 562.5us
			else if(IR_PULSE(IR_NEC_BIT,delta))
			{			
				if(IR_State.Bit && !(IR_State.Bit%8))			// Save every 8 bits
				{
					IR_State.Byte[IR_State.Word++]=IR_State.Scratch; 
					
					if(IR_State.Word==4)
					{
						if(NEC2_IR)
						{
							// NEC1 protocol
							if(IR_State.Byte[2] == (uint8_t)(~IR_State.Byte[3]))
							{
								FIFO_Write((FIFO *)IR_Buf,IR_State.Byte[0]);
								FIFO_Write((FIFO *)IR_Buf,IR_State.Byte[1]);								
								FIFO_Write((FIFO *)IR_Buf,IR_State.Byte[2]);							
							}							
						}
						else
						{
							// NEC1 protocol
							if((IR_State.Byte[0] == (uint8_t)(~IR_State.Byte[1]))&&
								 (IR_State.Byte[2] == (uint8_t)(~IR_State.Byte[3])))
							{
								FIFO_Write((FIFO *)IR_Buf,IR_State.Byte[0]);
								FIFO_Write((FIFO *)IR_Buf,IR_State.Byte[2]);							
							}
						}
						
						IR_State.Bit = IR_Idle;						
					}						
				}					
			}
			else // garbage
				IR_State.Bit = IR_Idle;
		}
	}

	if(TIM14->SR & TIM_SR_UIF)
	{
		TIM14->SR &= ~TIM_SR_UIF;
		IR_State.Bit = IR_Idle;
	}
}
