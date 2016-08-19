/*
 * rtc.c
 *
 * Created: June-18-16, 6:02:33 PM
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
 
#include "rtc.h"
#include "font.h"

const char *WeekDay[] =
{ "Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday" };

void RTC_Init(void)
{	
	// Power interface enable
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	// Disable RTC Protection
	PWR->CR |= PWR_CR_DBP;
	// RTC clock enable, HSE/32
	RCC->BDCR |= RCC_BDCR_RTCEN | RCC_BDCR_RTCSEL_HSE;
	RTC_Set(RTC_INIT_DATE,RTC_INIT_TIME);
}

void RTC_Set(uint32_t Date,uint32_t Time)
{
	// Unlock RTC
	RTC->WPR = RTC_UNLOCK1;
	RTC->WPR = RTC_UNLOCK2;
	
	RTC->ISR |= RTC_ISR_INIT;
	
	// Wait for init mode to be ready
	while(!(RTC->ISR & RTC_ISR_INITF))
	  /* Wait */ ;
	
	RTC->PRER = ((RTC_PRESCALER_A-1)<<16)|(RTC_PRESCALER_S-1);
	
	// Set time
	RTC->TR = Time;
	RTC->DR = Date;

  #ifdef RTC_FORMAT_12HR
  RTC->CR |= RTC_CR_FMT;
  #endif
  
	// Exit init mode
	RTC->ISR &= ~RTC_ISR_INIT;
	
	// Lock RTC
	RTC->WPR = RTC_LOCK1;
	RTC->WPR = RTC_LOCK2;
}

void RTC_Print2BCD(uint8_t n)
{
	LCD_PutCh('0'+(n>>4)); 
  LCD_PutCh('0'+(n&0x0f));
}

void RTC_PrintTime(void)
{
  uint32_t Time, CR;
  
  Time = RTC->TR;
  CR = RTC->CR;
	
	if((CR & RTC_CR_FMT)&&!(Time &(RTC_TR_HT|RTC_TR_HU)))
		LCD_Puts("12");
	else
	{	
		LCD_PutCh('0'+((Time & RTC_TR_HT)>>20));
		LCD_PutCh('0'+((Time & RTC_TR_HU)>>16));
	}
	
	LCD_PutCh(TIME_SEPARATOR);
	RTC_Print2BCD((Time &(RTC_TR_MNT|RTC_TR_MNU))>>8);
	LCD_PutCh(TIME_SEPARATOR);	
	RTC_Print2BCD(Time &(RTC->TR &(RTC_TR_ST|RTC_TR_SU)));	

  if((CR & RTC_CR_FMT))
  //  LCD_Puts((Time & RTC_TR_PM)?"pm":"am");
		LCD_PutCh((Time & RTC_TR_PM)?SYMBOL_PM:SYMBOL_AM);
}

void RTC_PrintDate(void)
{ 
  uint32_t Date;
    
  Date = RTC->DR;

  LCD_Puts("20");
  // year
	RTC_Print2BCD((Date&(RTC_DR_YT|RTC_DR_YU))>>16);
  LCD_PutCh(DATE_SEPARATOR);
  
  // month
	RTC_Print2BCD((Date&(RTC_DR_MT|RTC_DR_MU))>>8);
  LCD_PutCh(DATE_SEPARATOR);
  
  // day
	RTC_Print2BCD(Date&(RTC_DR_DT|RTC_DR_DU));
}

void RTC_PrintWeekDay(uint8_t length)
{
	char const *str = WeekDay[(RTC->DR>>13)&0x07-1];
	
	while(*str && length--)
		LCD_PutCh(*str++);
}
