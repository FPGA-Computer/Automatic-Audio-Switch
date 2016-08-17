/*
 * gpio.h
 *
 * Created: March-12-16, 5:37:11 PM
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

#ifndef _GPIO_H_
#define _GPIO_H_

// STM32F030F4 GPIO

enum GPIO_A_PINS
{ 
	PA0,PA1,PA2,PA3,PA4,PA5,PA6,PA7,
  PA9=9,PA10=10,PA13=13,PA14=14
};

enum GPIO_B_PINS
{	
	PB1=1
};

enum GPIO_F_PINS
{
	PF0,PF1
};

#define PIN_F2(BIT,FUNC) 		((FUNC)<<(BIT*2))

// MODER
#define PIN_MODER_MASK(BIT)		PIN_F2(BIT,0x03)
#define PIN_ANALOG(BIT)				PIN_F2(BIT,0x03)
#define PIN_INPUT(BIT)				PIN_F2(BIT,0x00)
#define PIN_OUTPUT(BIT)				PIN_F2(BIT,0x01)
#define PIN_ALT(BIT)					PIN_F2(BIT,0x02)

// AFR[0]
#define PIN_AFRL(BIT,AF)			(AF<<((BIT)*4))
// AFR[1]
#define PIN_AFRH(BIT,AF)			PIN_AFRL(BIT-8,AF)

// OSPEEDR
#define PIN_OSPEED(BIT,SPEED) PIN_F2(BIT,SPEED)

// OTYPER
#define PIN_PUSHPULL(BIT)			0
#define PIN_OPENDRAIN(BIT)		(1<<BIT)

// PUPDR
#define PIN_PULLUP(BIT)				PIN_F2(BIT,0x01)
#define PIN_PULLDOWN(BIT)			PIN_F2(BIT,0x02)

// BSRR
#define PIN_SET(BIT)					(1<<BIT)
#define PIN_CLR(BIT)					(1<<(BIT+16))
#endif
