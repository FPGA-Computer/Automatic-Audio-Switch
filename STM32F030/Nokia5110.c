/*
 * Nokia5110.c
 *
 * Created: 09/07/2013 1:46:07 PM
 *  Author: K. C. Lee
 */ 

#include "hardware.h"
#include "font.h"

uint8_t TextAttr=0, CurCol, CurRow;
volatile uint8_t SPI_Wait;

void DMA1_Channel2_3_IRQHandler(void)
{
	if(DMA1->ISR & DMA_ISR_TCIF3)
	{	
		SPI_Wait = 0;
		DMA1->IFCR = DMA_IFCR_CTCIF3;
	}
}

const uint8_t LCD_InitCmd[]=
{ 
	LCD_CMD_FH1, LCD_Contrast|LCD_ContrastDefault, LCD_TempCtrl,
  LCD_SetBias, LCD_CMD_FH0, LCD_Disp_Off
 };

void SPI_DMA_Wait(void)
{
	while(SPI_Wait)
		__WFI();
  
  // wait until SPI finishes
  while(SPI1->SR & SPI_SR_BSY)
    /* wait */ ;
}

void SPI_Block_Fill(uint8_t Fill, uint16_t size)
{
  DMA1_Channel3->CCR &= ~DMA_CCR_EN;
  DMA1_Channel3->CMAR = (uint32_t) &Fill;
  DMA1_Channel3->CNDTR = size;
  
  // 8-bit memory/peripheral, write, enable
	SPI_Wait = 1;
  DMA1_Channel3->CCR = SPI_DMA_PRIORITY|DMA_CCR_DIR|DMA_CCR_TCIE|DMA_CCR_EN;
  SPI_DMA_Wait();  
}

void SPI_Block_Write(const uint8_t *ptr, uint16_t size)
{
  DMA1_Channel3->CCR &= ~DMA_CCR_EN;
  DMA1_Channel3->CMAR = (uint32_t) ptr;
  DMA1_Channel3->CNDTR = size;
  SPI_Wait = 1;
	
  // 8-bit memory/peripheral, memory increment, write, enable, complete IRQ
  DMA1_Channel3->CCR = SPI_DMA_PRIORITY|DMA_CCR_MINC|DMA_CCR_DIR|DMA_CCR_TCIE|DMA_CCR_EN;
  SPI_DMA_Wait();  
}

void SPI_ByteWrite(uint8_t byte)
{
  SPI_Block_Write(&byte,1);
}

void LCD_Init(void)
{ 
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	
	// SPI Master, Software NSS, Slave select, MSB first, 
	// SCK=48MHz/16=3MHz, CPOL=0, CHPA=0
	SPI1->CR1 = SPI_CR1_MSTR|SPI_CR1_SSM|SPI_CR1_SSI|SPI_BR;
	
	// Data size = 8-bit, Motorola SPI mode, No NSS
	SPI1->CR2 = SPI_CR2_DS_2|SPI_CR2_DS_1|SPI_CR2_DS_0|SPI_CR2_TXDMAEN;	

	// Enable SPI
	SPI1->CR1|= SPI_CR1_SPE;
  
	NVIC_SetPriority(DMA1_Ch2_3_DMA2_Ch1_2_IRQn,LCD_DMA_IRQ_PRIORITY);
  NVIC_EnableIRQ(DMA1_Ch2_3_DMA2_Ch1_2_IRQn);
	
	// DMA Ch3 - SPI
	DMA1_Channel3->CPAR = (uint32_t) &SPI1->DR;

  LCD_CmdMode();
  SPI_Block_Write(LCD_InitCmd,sizeof(LCD_InitCmd));

  LCD_Cls();
  SPI_ByteWrite(LCD_Disp_On);
 }

void LCD_CORD_XY(uint8_t X, uint8_t Y)
{
  LCD_CmdMode();
  SPI_ByteWrite(LCD_Addr_X | (X & 0x7f));
  SPI_ByteWrite(LCD_Addr_Y | (Y & 0x07));
 }

// Text
void LCD_Moveto(uint8_t Col, uint8_t Row)
{
  LCD_CORD_XY(CurCol=Col*LCD_SYM_WIDTH,CurRow=Row);
 }

void LCD_Cls(void)
{ 
  LCD_CORD_XY(0,0);
  LCD_DataMode();

  SPI_Block_Fill(0x0,LCD_MAX_X*LCD_MAX_Y);
  LCD_Home();
}

void Cursor_NewLine(void)
{ 
  CurCol=0;
  CurRow++;

  if(CurRow==LCD_MAXROW)
  CurRow=0;
 }

void LCD_HalfWidthCh(uint8_t Ch)
{
  SPI_Block_Write(&Font_5x8[(Ch-Font_5x8_CH_LOW)*LCD_TEXT_WIDTH+1],LCD_SYM_WIDTH/2);
 }

void LCD_PutCh(uint8_t Ch)
{ const uint8_t *ptr;
  uint8_t i, Width;

  if((Ch>=Font_5x8_CH_LOW)&&(Ch<=Font_5x8_CH_HIGH))
  { 
    LCD_DataMode();	
    ptr = &Font_5x8[(Ch-Font_5x8_CH_LOW)*LCD_TEXT_WIDTH];		// Width = 5
    Width=LCD_TEXT_WIDTH;

    if(TextAttr==TextAttr_Normal)
      SPI_Block_Write(ptr,Width);
    else if(TextAttr==TextAttr_Invert)      
      for(i=Width;i;i--)
				SPI_ByteWrite(*ptr++);

    if(!(Ch & 0x80))
      SPI_ByteWrite((TextAttr==TextAttr_Invert)?0xff:0);

    CurCol+=LCD_SYM_WIDTH;

    // keep track of text cursor
    if(CurCol>LCD_MAX_X)
    { 
			CurCol%=LCD_MAX_X;
      Cursor_NewLine();
     }
   }
	else
		switch(Ch)
		{
			case Ch_NewLine:
				Cursor_NewLine();
				LCD_Moveto(CurCol,CurRow);
				break;
		}
 }

void LCD_Puts(const char *str)
{ 
  while (*str)
    LCD_PutCh(*str++);
 }
