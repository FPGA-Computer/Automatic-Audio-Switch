/*
 * rcc.c
 *
 * Created: March-14-16, 4:41:54 PM
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
 
#include "stm32f0xx.h"
#include "core_cm0.h"
#include "audio.h"

// Runs before main() set up system clock

void SystemInit(void)
{
	// Change FLASH prefetch = on, wait state to 1
               // Device header
	FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;

	//Set PLL to 48MHz with external 12MHz OSC	
#ifdef HSE
	// SYSCLK = HSE * 4 = 48MHz
	RCC->CR |= RCC_CR_HSEON|HSE_BYPASS;
	
	while(!(RCC->CR & RCC_CR_HSERDY))
		/* wait for HSE ready*/;
	
	// PLL Src = HSE, Multiplier = x2
	RCC->CFGR = RCC_CFGR_PLLSRC_1|CFGR_PLL_MULT;
	RCC->CR |= RCC_CR_PLLON;
	
	// Wait for PLL ready
	while(!(RCC->CR & RCC_CR_PLLRDY))
		;
	// Switch to PLL as clock source
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	
	// wait for clock switching
	while((RCC->CFGR & RCC_CFGR_SWS)!= RCC_CFGR_SWS_PLL)
	 ;
	
	// Turn off HSI
	RCC->CR &= ~RCC_CR_HSION;
	
#else
	// SYSCLK = HSI/2 * CLK_PLL_MULT (13 for 50.5MHz)
	// Trim HSI down to about 3.87MHz
	// dotclock = 25.16MHz on mine

	RCC->CFGR = CFGR_PLL_MULT;
	RCC->CR = (RCC->CR & ~RCC_CR_HSITRIM)|(HSI_TRIM<<3)|RCC_CR_PLLON;
	
	// Wait for PLL ready
	while(!(RCC->CR & RCC_CR_PLLRDY))
		;
	// Switch to PLL as clock source
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	
	// wait for clock switching
	while((RCC->CFGR & RCC_CFGR_SWS)!= RCC_CFGR_SWS_PLL)
	 ;
#endif
}
