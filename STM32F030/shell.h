/*
 * Shell.h
 *
 * Created: 08/09/2012 10:10:18 PM
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

#ifndef SHELL_H_
#define SHELL_H_

#include <inttypes.h>

#define Char_BS			0x08
#define Char_CR			0x0d
#define Char_LF			0x0a

#define PROMPT			"$ "

#define LINEBUFSIZE	24
#define Desc_Col		10

typedef struct
{ 
	const char *Token;
  const char *Description;
  void (* func)(void);
 } CMD_TBL;

void Shell_Init(void);
void Shell_Term_Task(void);
void Shell_Exec(void);
uint8_t ExecCmd(void);

unsigned int GetParm(void);

void Help(void);
extern const CMD_TBL Command_Table[];
#endif /* SHELL_H_ */

