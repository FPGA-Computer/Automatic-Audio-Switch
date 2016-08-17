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
FFT_t FFT;
int32_t Peak_Decay;

// 0VU is 1.737V peak, -51 to +3VU in 3dB steps scaled to 2047 
const uint32_t dB_Table[]=
{
	0xC2,0x112,0x183,0x223,0x305,0x444,0x607,0x884,
	0xC07,0x10FE,0x1800,0x21E7,0x2FE4,0x43A5,0x5F8E,0x86F9,
	0xBEA8,0x10D50,0x17C6A,ADC_MAX*ADC_BLOCK_SIZE
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

const uint32_t fft_dBScale[] =
{
	0x7FFFFF,0x5A9DF7,0x4026E7,0x2D6A86,0x2026F2,0x16C310,0x101D3F,0xB6873,
	0x81385,0x5B7B1,0x40C37,0x2DD95,0x20756,0x16FA9,0x10449,0xB844,
	0x8273,0x5C5A,0x4161,0x2E49,0x20C4,0x1732,0x106C,0xBA0,
	0x83B,0x5D3,0x420,0x2EB,0x211,0x176,0x109,0xBB,
	0x84,0x5E,0x42,0x2F,0x21,0x17,0x10
};

const uint8_t fft_noise_floor[] =
{
	0x68,0x20,0x20,0x10,0x10,0x08,0x04,0x04
};

void Audio_Init(void)
{
	Peak_Decay = dB_Table[dB_TBL_ENTERIES-2]/PEAK_DECAY_SAMPLES;
	memset(&Audio_Data,0,sizeof(Audio_Data));
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
	
  for(i=0;i<dB_TBL_ENTERIES;i++)
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
	
	Draw_VUBar(Audio_Data.Average_Volume[Audio_Data.Selected*ADC_MAX_CH+Left_Ch],
						 Audio_Data.Peak_Volume[Audio_Data.Selected*ADC_MAX_CH+Left_Ch]);
	
	LCD_CORD_XY(VU_COL,VU_R_ROW);
	LCD_DataMode();
	
	Draw_VUBar(Audio_Data.Average_Volume[Audio_Data.Selected*ADC_MAX_CH+Right_Ch],
						 Audio_Data.Peak_Volume[Audio_Data.Selected*ADC_MAX_CH+Right_Ch]);	
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
		
		if((Volume != Audio_Loud) && Audio_Data.Loud_Cnt[i])
			Audio_Data.Loud_Cnt[i]--;
		if((Volume != Audio_Detect) && Audio_Data.Detect_Cnt[i])
			Audio_Data.Detect_Cnt[i]--;
		
		Audio_Data.Loudness |= (Volume &0x03)<<(i*2);
	}
}

void Spectrum(void)
{
	uint16_t i, start, DC_Offset;
	int16_t *Cur;
	uint32_t *Mag;
	int16comp_t *Sp;
	
	if(Audio_Data.Conv_HalfDone)
		Cur = &Audio_Data.AudioBuffer[0];
	else
		Cur = &Audio_Data.AudioBuffer[ADC_MAX_CH*ADC_BLOCK_SIZE];		

	start = Audio_Data.Selected*ADC_MAX_CH;
	DC_Offset = Audio_Data.Averages[start]+Audio_Data.Averages[start+1];

	Sp = &FFT.fft_data[0];
	Cur += start;
	
	memset(&FFT,0,sizeof(FFT));
	
	// Mix left/right channels, subtract offset and hanning window	
	for(i=0;i<ADC_BLOCK_SIZE;i++)
	{  
		Sp->r = ((int32_t)(Cur[0]+Cur[1]-DC_Offset)*(int32_t)Cos_128pt[i])>>12;
		Sp++;
		Cur += ADC_MAX_CH;
	}
	
	int16fft_exec(FFT.fft_data);
	
	Mag = FFT.fft_mag;
	
	for(i=0;i< N_FFT / 2;i++)
	{
		Sp = &FFT.fft_data[Tbl_brev[i]];
		*Mag++ = (uint32_t)(Sp->r*Sp->r) + (uint32_t)(Sp->i*Sp->i);
	}
	
	// surpress the noise floor
	for(i=0;i<sizeof(fft_noise_floor);i++)
	  if(FFT.fft_mag[i] >= fft_noise_floor[i])
		  FFT.fft_mag[i] -= fft_noise_floor[i];
		else
			FFT.fft_mag[i] = 0;
}

void Plot_Spectrum(void)
{
	uint16_t i,j,k,bm;
	int16_t l;
	uint32_t *Mag, Peak, Average;
	
	memset(FFT.LCD_Buffer,0,sizeof(FFT.LCD_Buffer));
	
	// plot VU
	for(j=0;j<ADC_CH_PER_SRC;j++)
	{
		Peak = Audio_Data.Peak_Volume[Audio_Data.Selected+(1-j)];
		Average = Audio_Data.Average_Volume[Audio_Data.Selected+(1-j)];	
		
		if(Peak < Average)
			Peak = Average;
	
		// Peak -> dB lookup		
		for(l=dB_TBL_ENTERIES-1;l>=0;l--)
			if((Peak >= dB_Table[l])&&(Peak <= dB_Table[l+1]))
				break;
		
		i=SPECTRUM_ROWS-l/(LCD_PIX_PER_ROW/2)-1;		
		bm = 1<<((LCD_PIX_PER_ROW/2-l%(LCD_PIX_PER_ROW/2))*2+1);
		bm |=	bm>>1;
			
		for(k=0;k<SPECTRUM_VU_WIDTH;k++)
			FFT.LCD_Buffer[i][SPECTRUM_VU_COL+j*(SPECTRUM_VU_WIDTH+1)+k]= bm;			

		// average -> dB lookup
		for(l=dB_TBL_ENTERIES-1;l>=0;l--)
		  if(Average >= dB_Table[l])
				break;

		bm = 0;
		i=SPECTRUM_ROWS-l/(LCD_PIX_PER_ROW/2)-1;			

		for(k=0;k<(l%(LCD_PIX_PER_ROW/2));k++)
			bm |= 1<<((LCD_PIX_PER_ROW/2-k)*2+1);

		// Draw remainder	
		for(k=0;k<SPECTRUM_VU_WIDTH;k++)
			FFT.LCD_Buffer[i][SPECTRUM_VU_COL+j*(SPECTRUM_VU_WIDTH+1)+k] |= bm;			
		
		// draw full column
		for(i++;i<=SPECTRUM_ROWS;i++)
			for(k=0;k<SPECTRUM_VU_WIDTH;k++)
				FFT.LCD_Buffer[i][SPECTRUM_VU_COL+j*(SPECTRUM_VU_WIDTH+1)+k] |= 0xaa;			
	}

	#ifdef SPECTRUM_AUTOSCALE
	// Spectrum width
	Mag = &FFT.fft_mag[ADC_BLOCK_SIZE/2-1];
	
	for(i=ADC_BLOCK_SIZE/2;i;i--)
	  if(*(Mag--) >=fft_dBScale[sizeof(fft_dBScale)/sizeof(uint32_t)-1])
			break;
	
	// Scaling spectrum display
	l=(i<SPECTRUM_VU_COL/2)?2:1;
	#else	
		l=1;
	#endif
		
	// Plot Spectrum		
	Mag = &FFT.fft_mag[SPECTRUM_START];	
	for(j=0;j<SPECTRUM_END;j+=l)
	{
		for(i=0;i<SPECTRUM_ROWS;i++)
		{
			bm = 0xff;
			
			for(k=0;k<LCD_PIX_PER_ROW;k++)
			{
			  if(*Mag>=fft_dBScale[i*LCD_PIX_PER_ROW+k])
					break;

				bm &= ~(1<<k);
			}
			FFT.LCD_Buffer[i][j] |= bm;
			
			#ifdef SPECTRUM_WIDEBAR
			FFT.LCD_Buffer[i][j+1] |= bm;
			#endif
		}
		Mag++;
	}
	
	LCD_CORD_XY(SPECTRUM_COL,SPECTRUM_ROW);
	LCD_DataMode();
	SPI_Block_Write((const uint8_t *)FFT.LCD_Buffer,sizeof(FFT.LCD_Buffer));
	Audio_Data.Spectrum_Blank = 0;
}

void Blank_Spectrum(void)
{
	if(!Audio_Data.Spectrum_Blank)
	{
		LCD_CORD_XY(SPECTRUM_COL,SPECTRUM_ROW);
		LCD_DataMode();	
		SPI_Block_Fill(0,SPECTRUM_ROWS*LCD_MAX_X);
		Audio_Data.Spectrum_Blank = 1;
	}
}
