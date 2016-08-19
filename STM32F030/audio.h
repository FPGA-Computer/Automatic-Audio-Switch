/*
 * audio.h
 *
 * Created: June-16-16, 8:34:16 AM
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

#ifndef _AUDIO_H_
#define _AUDIO_H_

#include "hardware.h"
#include "intfft.h"

enum Audio_Mux
{
	Source_PC=0, Source_TV=1
};

void Audio_Init(void);
void Audio_Processing(void);
void Draw_VU_Legend(void);
void Draw_VUBar(int32_t Vin, int32_t Peak);
void Draw_VUBars(void);
void Spectrum(void);
void Plot_Spectrum(void);
void Blank_Spectrum(void);

enum Audio_Ch
{ Left_Ch=0, Right_Ch=1 };

enum Audio_Loudness
{ Audio_None=0, Audio_Sporadic, Audio_Detect, Audio_Loud };

#define AUDIO_LOUDNESS_MASK Audio_Loud
#define AUDIO_LOUDNESS_BITS 2

#define AUDIO_LOUDNESS_CH(X) ((Audio_Data.Loudness>>((X)*AUDIO_LOUDNESS_BITS))&AUDIO_LOUDNESS_MASK)

enum Audio_State
{
	Audio_Idle = 0, Audio_PC_Norm, Audio_PC_Loud, Audio_TV_Norm, Audio_TV_Loud
};

typedef struct
{
	int16_t		AudioBuffer[ADC_MAX_CH*ADC_AUDIO_SAMPLES];
	uint32_t	Average_Volume[ADC_MAX_AUD_CH];
	uint32_t	Peak_Volume[ADC_MAX_AUD_CH];
	int16_t 	Averages[ADC_MAX_CH];
	uint16_t 	Detect_Cnt[ADC_MAX_SRC];
	uint16_t 	Loud_Cnt[ADC_MAX_SRC];
	uint8_t		Loudness:ADC_MAX_SRC*AUDIO_LOUDNESS_BITS;
	uint8_t		Display_Mode:2;
	uint8_t		State:3;
	uint8_t		Selected:1;
	uint8_t		Spectrum_Blank:1;
	volatile 	uint8_t Conv_Done:1;
	volatile	uint8_t Conv_HalfDone:1;	
} Audio_t;

#pragma anon_unions

typedef struct
{
	union
	{ 
		int16comp_t fft_data[ADC_BLOCK_SIZE];	
	  uint8_t LCD_Buffer[SPECTRUM_ROWS][LCD_MAX_X];	
	};
	uint32_t fft_mag[ADC_BLOCK_SIZE/2];
} FFT_t;

extern Audio_t Audio_Data;
extern const uint32_t dB_Table[];
extern const uint32_t fft_dBScale[];

#define dB_TBL_ENTERIES (sizeof(dB_Table)/sizeof(int32_t)-1)

#endif
