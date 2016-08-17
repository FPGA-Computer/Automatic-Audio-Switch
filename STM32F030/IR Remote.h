/*
 * IR Remote.h
 *
 * Created: June-18-16, 6:05:57 PM
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

#ifndef _IR_REMOTE_H_
#define _IR_REMOTE_H_

#include "hardware.h"

typedef struct 
{
	uint8_t		Byte[4];
	uint8_t		Scratch;
	int8_t		Bit;
	uint8_t		Word;
} IR_Data;

extern IR_Data IR_State;
extern FIFO_Data_t IR_Buf[];
void IR_Init(void);

enum IR_Bit
{
	IR_Idle=-3, IR_Start=-2, IR_Lead=-1, 
	IR_Byte0_0 = 0
};
#define IR_FIFO_Size		4
#define IR_IRQ_PRIORITY	3

#define TIM14_PRESCALER	12

// in ns
#define TIM14_RES				250
// 9ms
#define IR_NEC_PRE			(9000000UL/TIM14_RES)
// 4.5ms space
#define IR_NEC_START		(4500000UL/TIM14_RES)
// 562.5us pulse and space for '0'
#define IR_NEC_BIT			(562500UL/TIM14_RES)
// 562.5us pulse and space for '0'
#define IR_NEC_BIT0			(562500UL/TIM14_RES)
// 1.6875ms space for '1'
#define IR_NEC_BIT1			(1687500UL/TIM14_RES)

#define IR_PULSE_TOL		0.1

#define IR_PULSE_MIN(X)	((X)*(1-IR_PULSE_TOL))
#define IR_PULSE_MAX(X)	((X)*(1+IR_PULSE_TOL))

#define IR_PULSE(X,Y)		(((Y)>=IR_PULSE_MIN(X))&&((Y)<=IR_PULSE_MAX(X)))

#define NEC2_IR 1

#endif
