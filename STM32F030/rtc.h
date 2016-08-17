/*
 * hardware.h
 *
 * Created: June-18-16, 6:03:09 PM
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

#ifndef _RTC_H_
#define _RTC_H_

#include "hardware.h"

#define RTC_PRESCALER_A		100L
#define RTC_PRESCALER_S		(HSE_CLOCK/(32L*RTC_PRESCALER_A))

#define RTC_UNLOCK1				0xca
#define RTC_UNLOCK2				0x53
#define RTC_LOCK1					0xfe
#define RTC_LOCK2					0x64

#define RTC_INIT_TIME			0x00
#define RTC_INIT_DATE			0x00162701

void RTC_Init(void);
void RTC_Set(uint32_t Date,uint32_t Time);
void RTC_PrintTime(void);
void RTC_PrintDate(void);
void RTC_PrintWeekDay(uint8_t length);
#endif
