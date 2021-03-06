/*
 * audio.c
 *
 * Created: June-20-16, 5:45:54 PM
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
#include "intfft.h"

Audio_t Audio_Data;
PlotData_t Plot_Data;
int32_t Peak_Decay;

// 0VU is 1.737V peak, -51 to +3VU in 3dB steps scaled to 2047 
const uint32_t dB_Table[]=
{
	0,
	0x0000C2,0x000112,0x000183,0x000223,
	0x000305,0x000444,0x000607,0x000884,
	0x000C07,0x0010FE,0x001800,0x0021E7,
	0x002FE4,0x0043A5,0x005F8E,0x0086F9,
	0x00BEA8,0x010D50,0x017C6A,
	ADC_MAX*ADC_BLOCK_SIZE
};

const uint32_t fft_dBScale[] =
{
	0,
	0x000002F,0x0000042,0x000005E,0x0000085,
	0x00000BC,0x000010A,0x0000178,0x0000213,
	0x00002EF,0x0000425,0x00005DA,0x0000845,
	0x0000BAE,0x0001080,0x000174E,0x00020EC,
	0x0002E81,0x00041B1,0x0005CCA,0x0008312,
	0x000B924,0x0010585,0x0017169,0x00209CE,
	0x002E112,0x0041124,0x005BEA6,0x0081D59,
	0x00B7656,0x01030DC,0x016DEC5,0x0204E15,
	0x02DA1CD,0x04074FC,0x05B0C43,0x0809BCB,
	0x0B5AA19,0x1009B9C,0x16A77DD,
	0x1FFFFFF,
};

// Hanning Cosine table
const int16_t Cos_128pt[] =
{
	0x0000,0x0013,0x004E,0x00B1,0x013A,0x01EA,0x02C1,0x03BD,
	0x04DF,0x0624,0x078E,0x091A,0x0AC9,0x0C98,0x0E86,0x1094,
	0x12BE,0x1505,0x1765,0x19DF,0x1C71,0x1F18,0x21D4,0x24A2,
	0x2781,0x2A70,0x2D6B,0x3072,0x3383,0x369B,0x39B9,0x3CDB,
	0x3FFF,0x4323,0x4645,0x4963,0x4C7B,0x4F8C,0x5293,0x558E,
	0x587D,0x5B5C,0x5E2A,0x60E6,0x638D,0x661F,0x6899,0x6AF9,
	0x6D40,0x6F6A,0x7178,0x7366,0x7535,0x76E4,0x7870,0x79DA,
	0x7B1F,0x7C41,0x7D3D,0x7E14,0x7EC4,0x7F4D,0x7FB0,0x7FEB,
	0x7FFF,0x7FEB,0x7FB0,0x7F4D,0x7EC4,0x7E14,0x7D3D,0x7C41,
	0x7B1F,0x79DA,0x7870,0x76E4,0x7535,0x7366,0x7178,0x6F6A,
	0x6D40,0x6AF9,0x6899,0x661F,0x638D,0x60E6,0x5E2A,0x5B5C,
	0x587D,0x558E,0x5293,0x4F8C,0x4C7B,0x4963,0x4645,0x4323,
	0x3FFF,0x3CDB,0x39B9,0x369B,0x3383,0x3072,0x2D6B,0x2A70,
	0x2781,0x24A2,0x21D4,0x1F18,0x1C71,0x19DF,0x1765,0x1505,
	0x12BE,0x1094,0x0E86,0x0C98,0x0AC9,0x091A,0x078E,0x0624,
	0x04DF,0x03BD,0x02C1,0x01EA,0x013A,0x00B1,0x004E,0x0013,
};

const uint8_t dB_bar_Legend[]=
{
	0x78,0x80,0x40,0x38,0x00,0x78,0x80,0x81,
	0x78,0x00,0x20,0x21,0x00,0x38,0x20,0xfb,
	0x00,0xb8,0xa8,0xe9,0x00,0x20,0x20,0x01,
	0xa8,0xa8,0xf8,0x03,0xf8,0xa8,0xe8,0x01,
	0x00,0x20,0x20,0x01,0xe8,0xa8,0xb8,0x03,
	0x08,0x08,0xf8,0x01,0x00,0x00,0x00,0x21,
	0x20,0x00,0xf8,0x03,0xf8,0xa8,0xf8,0x01,
	0x00,0x00,0x00,0x21,0x20,0x00,0xb8,0xab,
	0xf8,0x00,0x00,0x01,0x00,0x00,0x00,0x01,
	0x00,0x00,0xf8,0x8b,0xf8,0x00,0x00,0x01
};

const uint8_t dB_bar_off[]= 	{ 0x00,0x00,0x00,0x00 };
const uint8_t dB_bar_on[]= 		{ 0x00,0x7f,0x7f,0x7f };
const uint8_t dB_bar_Peak[]= 	{ 0x00,0x3e,0x1c,0x08 };

void Audio_Init(void)
{
	Peak_Decay = dB_Table[dB_TBL_ENTERIES-2]/PEAK_DECAY_SAMPLES;
	memset(&Audio_Data,0,sizeof(Audio_Data));
}

// binary search table lookup
int8_t Lookup(const uint32_t Value, const uint32_t *Table, uint16_t Last)
{
  uint16_t Low=0, Mid;
	
  while(Low<=Last)
	{
		Mid = (Low+Last)/2;
		
		if(Table[Mid]<=Value)
		{
			if(Value<Table[Mid+1])
			  return(Mid);

			Low = Mid+1;
		}
		else
			Last = Mid-1;
	}
	return(0);
}

void Draw_VU_Legend(void)
{
	LCD_Moveto(0,VU_L_ROW);	
	LCD_DataMode();
	LCD_PutCh('L');
	LCD_Moveto(0,VU_R_ROW);
	LCD_DataMode();
	LCD_PutCh('R');
	LCD_Moveto(0,VU_LEGEND_Y);
	LCD_DataMode();
	SPI_Block_Write(dB_bar_Legend,sizeof(dB_bar_Legend));
}

void Draw_VUBar(int32_t Avg, int32_t Peak)
{ 
  uint8_t i;
  
	if(Peak < Avg)
		Peak = Avg;
	
  for(i=1;i<dB_TBL_ENTERIES;i++)
  {
    if((Peak >= dB_Table[i])&(Peak < dB_Table[i+1]))
      SPI_Block_Write(dB_bar_Peak,sizeof(dB_bar_Peak));
    else
      SPI_Block_Write((Avg >= dB_Table[i])?dB_bar_on:dB_bar_off,sizeof(dB_bar_on));
  }
}

void Draw_VUBars(void)
{
	LCD_CORD_XY(VU_COL,VU_L_ROW);
	LCD_DataMode();
	
	Draw_VUBar(Audio_Data.Average_Volume[Audio_Data.Selected*ADC_CH_PER_SRC+Left_Ch],
						 Audio_Data.Peak_Volume[Audio_Data.Selected*ADC_CH_PER_SRC+Left_Ch]);
	
	LCD_CORD_XY(VU_COL,VU_R_ROW);
	LCD_DataMode();
	
	Draw_VUBar(Audio_Data.Average_Volume[Audio_Data.Selected*ADC_CH_PER_SRC+Right_Ch],
						 Audio_Data.Peak_Volume[Audio_Data.Selected*ADC_CH_PER_SRC+Right_Ch]);	
}

// This sums the raw ADC samples in the buffer
void Audio_Processing(void)
{
	int16_t i,j, *Cur, *Start, Volume;
	int32_t Sum[ADC_MAX_CH]={0}, Volumes[ADC_MAX_AUD_CH]={0};
	
	if(Audio_Data.Conv_HalfDone)
		Start = Cur = &Audio_Data.AudioBuffer[0];
	else
		Start = Cur = &Audio_Data.AudioBuffer[ADC_MAX_CH*ADC_BLOCK_SIZE];
	
	// Summing loop
	for(i=ADC_BLOCK_SIZE;i;i--)
	{
		Sum[0]+= *Cur++;
		Sum[1]+= *Cur++;
		Sum[2]+= *Cur++;
		Sum[3]+= *Cur++;
		Sum[4]+= *Cur++;
	}
	
	// Offset = Averages
	for(i=0;i< ADC_MAX_CH;i++)
		Audio_Data.Averages[i] = Sum[i]/ADC_BLOCK_SIZE;

	Cur = Start;

	// Volume = sum(ABS(Values-offset))
	for(i=ADC_BLOCK_SIZE;i;i--)
	{
		Volume = *Cur++ - Audio_Data.Averages[0];
		Volumes[0]+=(Volume>=0)?Volume:-Volume;
		Volume = *Cur++ - Audio_Data.Averages[1];
		Volumes[1]+=(Volume>=0)?Volume:-Volume;
		Volume = *Cur++ - Audio_Data.Averages[2];
		Volumes[2]+=(Volume>=0)?Volume:-Volume;
		Volume = *Cur++ - Audio_Data.Averages[3];
		Volumes[3]+=(Volume>=0)?Volume:-Volume;
		// Skip over button
		Cur++;
	}
	
	// Average volume
	for(j=0;j<ADC_MAX_AUD_CH;j++)
	{
		Audio_Data.Average_Volume[j] = (int32_t)(Audio_Data.Average_Volume[j]*(ADC_AVERAGES-1) +
																   Volumes[j])/ADC_AVERAGES;
		
		if(Volumes[j]>=(int32_t)Audio_Data.Peak_Volume[j])
			Audio_Data.Peak_Volume[j] = Volumes[j];
		else if((int32_t)Audio_Data.Peak_Volume[j]>=Peak_Decay)
			Audio_Data.Peak_Volume[j]-= Peak_Decay;
		else
			Audio_Data.Peak_Volume[j] = 0;
	}
	
	// Threshold detection
	Audio_Data.Loudness = 0;
	
	for(i=0;i<ADC_MAX_SRC;i++)
	{	
		Volume = 0;
		
		for(j=0;j<ADC_CH_PER_SRC;j++)
		{
			if(Volumes[i*ADC_CH_PER_SRC+j]>=LOUDNESS_THRESHOLD)
			{ 
				Volume = Audio_Loud;
				Audio_Data.Loud_Cnt[i] = LOUDNESS_TIMEOUT;
			}
			else if(Volumes[i*ADC_CH_PER_SRC+j]>=AUDIO_THRESHOLD)
			{
				Volume = Audio_Detect;
				Audio_Data.Detect_Cnt[i] = AUDIO_TIMEOUT;
			}
			else if(Audio_Data.Detect_Cnt[i]>AUDIO_COUNTDOWN_TH)
				Volume = Audio_Sporadic;
			else
				Volume =  Audio_None;
		}
		
		if(Audio_Data.Loud_Cnt[i])
		{ 
			Audio_Data.Loud_Cnt[i]--;
			Volume = Audio_Loud;
		}
		else if((Volume != Audio_Detect) && Audio_Data.Detect_Cnt[i])
			Audio_Data.Detect_Cnt[i]--;
		
		Audio_Data.Loudness |= (Volume &AUDIO_LOUDNESS_MASK)<<(i*AUDIO_LOUDNESS_BITS);
	}
}

void Spectrum(void)
{
	uint16_t i, start, DC_Offset;
	int16_t *Cur;
	uint8_t *Mag;
	int16comp_t *Sp;
	uint32_t Magnitude;
	
	if(Audio_Data.Conv_HalfDone)
		Cur = &Audio_Data.AudioBuffer[0];
	else
		Cur = &Audio_Data.AudioBuffer[ADC_MAX_CH*ADC_BLOCK_SIZE];		

	start = (Audio_Data.Selected)?ADC_CH_PER_SRC:0;
	DC_Offset = Audio_Data.Averages[start]+Audio_Data.Averages[start+1];

	Sp = &Plot_Data.fft_data[0];
	Cur += start;
	
	memset(&Plot_Data,0,sizeof(Plot_Data));
	
	// Mix left/right channels, subtract offset and hanning window	
	for(i=0;i<ADC_BLOCK_SIZE;i++)
	{  
		Sp->r = ((int32_t)(Cur[0]+Cur[1]-DC_Offset)*(int32_t)Cos_128pt[i])>>12;
		Sp++;
		Cur += ADC_MAX_CH;
	}
	
	int16fft_exec(Plot_Data.fft_data);
	
	Mag = Plot_Data.fft_mag;
	
	for(i=0;i< N_FFT / 2;i++)
	{
		Sp = &Plot_Data.fft_data[Tbl_brev[i]];
		Magnitude = (uint32_t)(Sp->r*Sp->r) + (uint32_t)(Sp->i*Sp->i);
		*Mag++ = Lookup(Magnitude,fft_dBScale,sizeof(fft_dBScale)/sizeof(uint32_t)-2);		
	}
}

void Plot_UV(uint8_t Value, uint8_t X, uint8_t Type)
{
  uint8_t Row, Col, Bitmap;
	
	Row=Value/SCALE_VU;
  Bitmap = ((Type==Plot_Average)?BITMAP_VU:BITMAP_PEAK)<<(8-(Value%SCALE_VU)*2);
	
	for(Col=SPECTRUM_VU_WIDTH;Col;Col--)
		Plot_Data.LCD_Buffer[SPECTRUM_ROWS-Row-1][X+Col] |= Bitmap;
	
	if(Type==Plot_Average)
	{
		while(Row--)
			for(Col=SPECTRUM_VU_WIDTH;Col;Col--)
				Plot_Data.LCD_Buffer[SPECTRUM_ROWS-Row-1][X+Col] |= BITMAP_VU;
	}
	else if(!(Value%SCALE_VU)&&Row)
	{			
		for(Col=SPECTRUM_VU_WIDTH;Col;Col--)
	    Plot_Data.LCD_Buffer[SPECTRUM_ROWS-Row][X+Col] |= BITMAP_PEAK;
	}
}

void Plot_SpectrumBin(uint8_t Value, uint8_t X)
{
  uint8_t Row, Bitmap;
	
	Row = Value/SCALE_SPECTRUM;
  Bitmap = BITMAP_SPECTRUM<<(8-(Value%SCALE_SPECTRUM));	
	Plot_Data.LCD_Buffer[SPECTRUM_ROWS-Row-1][X] |= Bitmap;
	
	while(Row--)
		Plot_Data.LCD_Buffer[SPECTRUM_ROWS-Row-1][X] |= BITMAP_SPECTRUM;	
}

void Plot_Spectrum(void)
{
	uint8_t update, Peak, Average;
	uint16_t i, j;
	
	update = Audio_Data.Loudness & (AUDIO_LOUDNESS_MASK<<Audio_Data.Selected*ADC_CH_PER_SRC);
	
	if(update)
		Spectrum();
	
	memset(Plot_Data.LCD_Buffer,0,sizeof(Plot_Data.LCD_Buffer));
	
	// plot VU
	for(j=0;j<ADC_CH_PER_SRC;j++)
	{
		// plot VU
		Average = Lookup(Audio_Data.Average_Volume[Audio_Data.Selected*ADC_CH_PER_SRC+(1-j)],
										 dB_Table,sizeof(dB_Table)/sizeof(uint32_t)-2);
		Peak = Lookup(Audio_Data.Peak_Volume[Audio_Data.Selected*ADC_CH_PER_SRC+(1-j)],
										 dB_Table,sizeof(dB_Table)/sizeof(uint32_t)-2);	
			
		if(Peak < Average)
			Peak = Average;
		
		Plot_UV(Average,SPECTRUM_VU_COL+j*(SPECTRUM_VU_WIDTH+1),Plot_Average);
		Plot_UV(Peak,SPECTRUM_VU_COL+j*(SPECTRUM_VU_WIDTH+1),Plot_Peak);	
	}
	
	// Plot Spectrum		
	for(i=0;i<SPECTRUM_END;i++)
		Plot_SpectrumBin(Plot_Data.fft_mag[i],i);
	
	LCD_CORD_XY(SPECTRUM_COL,SPECTRUM_ROW);
	LCD_DataMode();
	SPI_Block_Write((const uint8_t *)Plot_Data.LCD_Buffer,sizeof(Plot_Data.LCD_Buffer));
	Audio_Data.Spectrum_Blank = 0;
}

