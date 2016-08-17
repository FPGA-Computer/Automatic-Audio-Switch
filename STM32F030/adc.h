/*
 * adc.h
 *
 * Created: June-16-16, 8:42:40 AM
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

#ifndef _ADC_H_
#define _ADC_H_

#include <stdint.h>

void ADC_Init(void);
void ADC_Start(void);

// lowest priority
#define ADC_DMA_IRQ_PRIORITY	3

// lowest priority
#define ADC_DMA_PRIORITY			0

#define ADC_RES								0
#define ADC_ALIGN							0
#define ADC_DMA_MSIZE					DMA_CCR_MSIZE_0 
#define ADC_DMA_PSIZE					DMA_CCR_PSIZE_0
#define AUDIO_BITS						12

#include "hardware.h"

#define ADC_MAX							(1<<(AUDIO_BITS-1))

#endif
