/*
 * hardware.c
 *
 * Created: June-16-16, 8:38:27 AM
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
 
#include "hardware.h"
 
void Hardware_Init(void)
{
	// Enable GPIO, DMA
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN|RCC_AHBENR_GPIOBEN|RCC_AHBENR_GPIOFEN|RCC_AHBENR_DMAEN;
	// Enable SPI, SYSCFG, DBGMCU, TIM16
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN|RCC_APB2ENR_DBGMCUEN|RCC_APB2ENR_TIM16EN;
	
	// Assign GPIOA functions
	GPIOA->MODER = GPIOA_MODER;
	GPIOA->AFR[1] = GPIOA_AFR1;

	GPIOB->MODER = PIN_ALT(PB1);

	RTC_Init();
	LCD_Init();
	
	ADC_Init();
	IR_Init();
	
#ifdef DBG_SHELL
	USART_Init();
	Shell_Init();
#endif
}
