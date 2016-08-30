/*
 * hardware.h
 *
 * Created: July-10-16, 4:09:33 PM
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

#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdint.h>
#include "stm32f0xx.h"
#include "core_cm0.h"
#include <string.h>

void Task_Update_Volume(void);
void Task_ADC_Sample(void);
void UpdateTime(void);
void UpdateDisplay(void);
void ChangeDisplayMode(uint8_t Mode);
void Update_AudioSource(void);
void Draw_AudioSource_Label(void);

enum Display_Mode
{ Spectrum_Mode, VU_Mode };

#define AUDIO_SRC0_LABEL			"PC"
#define AUDIO_SRC1_LABEL			"TV"

// Time separator
#define TIME_SEPARATOR  			':'
// Date separator
#define DATE_SEPARATOR  			'-'

// 12 hours format
#define RTC_FORMAT_12HR

// Volume average time in ms
#define VOLUME_AVERAGE_PERIOD	300
// Peak volume decay time in ms
#define PEAK_VOLUME_DECAY			1500

#define ADC_AUDIO_SAMPLES			256
#define ADC_BLOCK_SIZE				(ADC_AUDIO_SAMPLES/2)
#define N_FFT									ADC_BLOCK_SIZE

#define ADC_SAMPLE_RATE				38400UL
#define ADC_SAMPLE_PERIOD			(CPU_CLOCK/ADC_SAMPLE_RATE)

#define ADC_DMA_IRQ_RATE			(ADC_SAMPLE_RATE/ADC_BLOCK_SIZE)
#define SCREEN_REFRESH_RATE		15UL
#define TICK_RELOAD						(ADC_DMA_IRQ_RATE/SCREEN_REFRESH_RATE)

#define AUDIO_TIMEOUT					1000
#define LOUDNESS_TIMEOUT			1000
#define AUDIO_COUNTDOWN_TH		(AUDIO_TIMEOUT/2)				

#define AUDIO_THRESHOLD 			0x94L

// relay threshold
//#define AUDIO_THRESHOLD 			0x01c0L
#define LOUDNESS_THRESHOLD		0x10FE

extern uint32_t Prev_Time;
extern uint8_t Ticks;

#define RTC_COL						4
#define RTC_DATE_ROW			0
#define RTC_TIME_ROW			1
#define RTC_ALT_ROW				0

#define AUDIO_SRC0_COL		3
#define AUDIO_SRC1_COL		10
#define AUDIO_SRC_ROW			2
#define AUDIO_SRC_COL			7

#define AUDIO_SRC_ALT_COL 10
#define AUDIO_SRC_ALT_ROW RTC_ALT_ROW

#define VU_COL						5
#define VU_L_ROW					3
#define VU_R_ROW					4
#define VU_LEGEND_Y				5

// Alternate VU bar
#define VU_BAR_WIDTH			3
#define VU_BAR_ALT_COL		1
#define VU_BAR_ALT_ROW		0
#define VU_BAR_MAJ_TIC		3

#define SPECTRUM_ROWS			5
#define SPECTRUM_ROW			1
#define SPECTRUM_COL			0

#define SPECTRUM_START		0
#define SPECTRUM_BIN			(LCD_MAX_X-SPECTRUM_WIDTH)
#undef	SPECTRUM_WIDEBAR 

#ifdef SPECTRUM_WIDEBAR
#define SPECTRUM_WIDTH		2
#else
#define SPECTRUM_WIDTH		1
#endif

#define SPECTRUM_VU_WIDTH	5
#define SPECTRUM_VU_COL		(LCD_MAX_X-(SPECTRUM_VU_WIDTH+1)*ADC_CH_PER_SRC)

#define MIN(X,Y)					(((X)<=(Y))?(X):(Y))
#define SPECTRUM_END			MIN(LCD_MAX_X,(ADC_BLOCK_SIZE/2-SPECTRUM_START)*SPECTRUM_WIDTH)

#undef SPECTRUM_AUTOSCALE

#endif
