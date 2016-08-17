/*
 * cmd.c
 *
 * Created: 08/09/2012 10:29:05 PM
 *  Author: User Unknown
 */ 

#include "shell.h"
#include "lib.h"

const CMD_TBL Command_Table[]= 
{ 
	{"HELP","Show a list of commands.",	Help},
  {0}		/* Marks end of list */
 };
