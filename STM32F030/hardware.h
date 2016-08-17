/*
 * hardware.h
 *
 * Created: March-14-16, 4:52:17 PM
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

#ifndef _HARDWARE_H_
#define _HARDWARE_H_

#include "main.h"
#include "gpio.h"

#define CPU_CLOCK					48000000UL
#define UART_BAUD					115200

#define HSE_CLOCK					12000000UL
#define HSE
#define HSE_BYPASS				0

// Enable Debug shell 
//#define DBG_SHELL
#undef DBG_SHELL

//#define ADC_RES_8
#define ADC_RES_12

// 12MHz clock x4 = 48MHz
#define CFGR_PLL_MULT					RCC_CFGR_PLLMUL4

#define LCD_DC								PA6
#define ADC_SRC_GROUP					(ADC_CHSELR_CHSEL0|ADC_CHSELR_CHSEL1|ADC_CHSELR_CHSEL2|ADC_CHSELR_CHSEL3|ADC_CHSELR_CHSEL4)

#define PEAK_DECAY_SAMPLES		((PEAK_VOLUME_DECAY*ADC_SAMPLE_RATE/ADC_BLOCK_SIZE)/1000)
#define ADC_AVERAGES					(VOLUME_AVERAGE_PERIOD*ADC_SAMPLE_RATE/ADC_BLOCK_SIZE/1000)

// Max # of audio source
#define ADC_MAX_SRC						2
// L/R channels
#define ADC_CH_PER_SRC				2
#define ADC_MAX_AUD_CH				(ADC_MAX_SRC*ADC_CH_PER_SRC)
// Max # of channels per conversion
#define ADC_MAX_CH						(ADC_MAX_AUD_CH+1)

#include "main.h"
#include "nokia5110.h"
#include "adc.h"
#include "audio.h"
#include "fifo.h"
#include "rtc.h"
#include "IR Remote.h"

void Hardware_Init(void);

enum ADC_Chs
{
	Audio_R1, Audio_L1, Audio_R0, Audio_L0, Buttons
};

#define IR_PORT			GPIOB
#define IR_PIN			PB1
#define IR_INP			(IR_PORT->IDR & (1<<IR_PIN))			

#ifdef DBG_SHELL
#include "serial.h"
#include "shell.h"

#define GPIOA_MODER (PIN_ALT(PA14)|PIN_ALT(PA13)|				/* SWCLK, SWDIO */	\
										 PIN_ALT(PA10)|PIN_ALT(PA9)|				/* RxD, TxD			*/	\
										 PIN_ALT(PA7)|PIN_OUTPUT(PA6)|			/* MOSI, D/C 		*/	\
										 PIN_ALT(PA5)|PIN_ANALOG(PA4)|			/* SCK ???			*/	\
										 PIN_ANALOG(PA3)|PIN_ANALOG(PA2)|		/* L0, R0				*/	\
		                 PIN_ANALOG(PA1)|PIN_ANALOG(PA0))		/* L1, R1				*/

#define CTRL1				0
#define CTRL0				0

#define GPIOA_AFR1	 (PIN_AFRH(PA10,1)|PIN_AFRH(PA9,1))	/* RxD, TxD 		*/

#else
#define GPIOA_MODER (PIN_ALT(PA14)|PIN_ALT(PA13)|				/* SWCLK, SWDIO */	\
										 PIN_OUTPUT(PA10)|PIN_OUTPUT(PA9)|	/* CTRL1, CTRL0	*/	\
										 PIN_ALT(PA7)|PIN_OUTPUT(PA6)|			/* MOSI, D/C 		*/	\
										 PIN_ALT(PA5)|PIN_ANALOG(PA4)|			/* SCK ???			*/	\
										 PIN_ANALOG(PA3)|PIN_ANALOG(PA2)|		/* L0, R0				*/	\
		                 PIN_ANALOG(PA1)|PIN_ANALOG(PA0))		/* L1, R1				*/
										 
#define CTRL1				PA10
#define CTRL0				PA9

#define GPIOA_AFR1	 0
#endif

#endif
