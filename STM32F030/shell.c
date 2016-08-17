/*
 * Shell.c
 *
 * Created: 08/09/2012 10:09:46 PM
 *  Author: User Unknown
 */ 

#include <stdio.h>
#include <stdint.h>
#include "lib.h"
#include "Shell.h"
#include "serial.h"

static char LineBuffer[LINEBUFSIZE];
static char *LinePtr;
static uint8_t Line_Pos;

void Shell_Init(void)
{ 
  Puts(PROMPT);  
  LineBuffer[0]=0;
  Line_Pos=0;
  LinePtr=NULL;
}

void Shell_Term_Task(void)
{ int ch;
  
	LinePtr=NULL;
  ch=Getc((FIFO *)RxBuf);
	
	if ((ch>=' ') && (Line_Pos < LINEBUFSIZE-1))
	{ 
		USART_PutChar(ch);
		LineBuffer[Line_Pos++]=ch;
	 }
	else if (ch==Char_BS)
	{ if (Line_Pos)
		{ 
			LineBuffer[Line_Pos--]=0;
			Puts("\x08 \x08");
		 }
	 }
	else if(ch==Char_CR)
	{ 
		LineBuffer[Line_Pos]=0;
		LinePtr=LineBuffer;
		NewLine();
		Shell_Exec();
	 } 
 }

void Shell_Exec(void)
{ 
  if(*LinePtr && !ExecCmd())
  { Puts("???\n");
    Help();
   }
  Shell_Init();       
 }

void SkipSpace(void)		/* Skip to first non-space character in line buffer */
{
  for(;*LinePtr&&IsWhiteSpc(*LinePtr);LinePtr++)
  /* */;
 }

// Match against strings in program space
char *Buf_FMatch(const char *string)
{ char *Buf=LinePtr; 

  while(*Buf && !IsWhiteSpc(*Buf))
    if(ToUpper(*Buf++)!=*string++)
      return(0);

  return(*string?NULL:Buf);
 }

uint8_t ExecCmd(void)
{ const CMD_TBL *tbl_ptr;
  char *Parm;

  for(tbl_ptr=Command_Table;tbl_ptr->Token;tbl_ptr++)
  { 
    Parm=Buf_FMatch(tbl_ptr->Token);
    
    if(Parm)
    { 
      LinePtr=Parm;
      SkipSpace();
      tbl_ptr->func();
      NewLine();
      return(1);
     }
   }
  return(0);
 }

// Get hex argument from line buffer
unsigned int GetParm(void)
{ unsigned int n;
  n=0;

  while(*LinePtr && !IsWhiteSpc(*LinePtr))
    n=(n<<4)|Hex(*LinePtr++);

  SkipSpace();
  return(n);
 }

void Help(void)
{ uint8_t Col;
  const CMD_TBL *tbl_ptr;

  Puts("Available commands:\n");
  
  for(tbl_ptr=Command_Table;tbl_ptr->Token;tbl_ptr++)
  { Col=Puts(tbl_ptr->Token);
    FillSpc(Desc_Col-Col);
    Puts(tbl_ptr->Description);
    NewLine();
   }
 }

