/*
 * main.c
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
#include "font.h"

uint32_t Prev_Time=0xffff;
uint8_t Ticks;

int main(void)
{ 
	Hardware_Init();
	Audio_Init();
	ChangeDisplayMode(/*VU_Mode*/Spectrum_Mode);
	ADC_Start();
	
	while(1)
	{	
		__WFI( 	);
		if(Audio_Data.Conv_Done)
		{ 		
			Audio_Data.Conv_Done = 0;
			
			Audio_Processing();
			UpdateDisplay();
		}
		
		if(FIFO_ReadAvail((FIFO*)IR_Buf))
		{
			Getc((FIFO*)IR_Buf);
		}
	}
}

void UpdateDisplay(void)
{
	if(Ticks)
		Ticks--;
	else
	{
		Ticks = TICK_RELOAD;
		UpdateTime();		
		Update_AudioSource();
		
		switch(Audio_Data.Display_Mode)
		{
			case VU_Mode:
				Draw_VUBars();	
				break;
			case Spectrum_Mode:
				if(!(Audio_Data.Loudness & (AUDIO_LOUDNESS_MASK<<Audio_Data.Selected*ADC_CH_PER_SRC)))
					Blank_Spectrum();
				else 				
				{
					Spectrum();
					Plot_Spectrum();
				}
				break;
		}	
	}
}

void ChangeDisplayMode(uint8_t Mode)
{
	LCD_Cls();
	Audio_Data.Display_Mode = Mode;
	
	if(Audio_Data.Display_Mode==VU_Mode)
	{	
		Draw_AudioSource_Label();
		Draw_VU_Legend();
	}
}

void UpdateTime(void)
{		
	if(Prev_Time != RTC->TR)
	{
		Prev_Time = RTC->TR;
		LCD_Home();
		
		if(Audio_Data.Display_Mode==VU_Mode)
		{
			RTC_PrintDate();
			LCD_PutCh(' ');
			RTC_PrintWeekDay(3);
			LCD_PutCh('\n');
		}
//		else
//			LCD_Moveto(0,RTC_ALT_ROW);
		
		RTC_PrintTime();
	}
}

void Draw_AudioSource_Label(void)
{	
	if(Audio_Data.Display_Mode==VU_Mode)
	{
		LCD_Moveto(AUDIO_SRC0_COL,AUDIO_SRC_ROW);
		LCD_Puts(AUDIO_SRC0_LABEL);
		LCD_Moveto(AUDIO_SRC1_COL,AUDIO_SRC_ROW);	
		LCD_Puts(AUDIO_SRC1_LABEL);
	}
}

void Audio_StateMachine(void)
{
	const uint32_t State_IO[] =
	{ PIN_SET(CTRL0),PIN_SET(CTRL0),PIN_SET(CTRL0),
		PIN_CLR(CTRL0),PIN_CLR(CTRL0)
	};
	
//	uint8_t OldState = Audio_Data.State;
	
	// State machine for switching
	switch(Audio_Data.State)
	{
		case Audio_Idle:
		case Audio_TV_Norm:
		case Audio_TV_Loud:
		  if(AUDIO_LOUDNESS_CH(Source_PC)==Audio_Loud)
				Audio_Data.State = Audio_PC_Loud;
			else if (AUDIO_LOUDNESS_CH(Source_PC)==Audio_Detect)
				Audio_Data.State = Audio_PC_Norm;
			else if (AUDIO_LOUDNESS_CH(Source_TV)==Audio_Loud)
				Audio_Data.State = Audio_TV_Loud;
			else if (AUDIO_LOUDNESS_CH(Source_TV)==Audio_Detect)
				Audio_Data.State = Audio_TV_Norm;
//		else
//  		Audio_Data.State = Audio_Idle;
			break;
			
		case Audio_PC_Norm:
		case Audio_PC_Loud:
		  if(AUDIO_LOUDNESS_CH(Source_PC)==Audio_Loud)
				Audio_Data.State = Audio_PC_Loud;
			else if (AUDIO_LOUDNESS_CH(Source_PC)==Audio_Detect)
				Audio_Data.State = Audio_PC_Norm;			
			else if (AUDIO_LOUDNESS_CH(Source_PC)==Audio_None)			
				Audio_Data.State = Audio_Idle;
			break;
	}
	
	// if(Audio_Data.State!=OldState)
	//{
	// State -> MUX setting
	GPIOA->BSRR = State_IO[Audio_Data.State];
	
	Audio_Data.Selected = ((Audio_Data.State == Audio_TV_Norm)||
												 (Audio_Data.State == Audio_TV_Loud))?1:0;
	
	//}
}

void Update_AudioSource(void)
{
	uint8_t i;
	
  const uint8_t Audio_Detect_Char[] =
	{ ' ',SYMBOL_SOUND_MAYBE,SYMBOL_SOUND_ON,SYMBOL_SOUND_LOUD };
	
	Audio_StateMachine();

	if(Audio_Data.Display_Mode==VU_Mode)
	{	
		for(i=0;i<ADC_MAX_SRC;i++)
		{
			LCD_Moveto(i*AUDIO_SRC_COL,AUDIO_SRC_ROW);
		  LCD_PutCh((Audio_Data.State==Audio_Idle)?' ':
								(Audio_Data.Selected==i)?SYMBOL_SPEAKER:' ');		
			LCD_PutCh(Audio_Detect_Char[AUDIO_LOUDNESS_CH(i)]);			
		}
	}
	else
	{
		LCD_Moveto(AUDIO_SRC_ALT_COL,AUDIO_SRC_ALT_ROW);
		LCD_PutCh((Audio_Data.State==Audio_Idle)?' ':SYMBOL_SPEAKER);
		LCD_PutCh(Audio_Detect_Char[AUDIO_LOUDNESS_CH(Audio_Data.Selected)]);
		LCD_Puts(Audio_Data.Selected?AUDIO_SRC1_LABEL:AUDIO_SRC0_LABEL);
	}
}
