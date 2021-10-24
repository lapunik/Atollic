/*
******************************************************************************
File:     main.c
Info:     Generated by Atollic TrueSTUDIO(R) 9.3.0   2019-03-02

The MIT License (MIT)
Copyright (c) 2019 STMicroelectronics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

******************************************************************************
*/

/* Includes */
#include "mpp_shield.h"
#include "stm_core.h"
#include "stdlib.h"


/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)
{

  /**
  *  IMPORTANT NOTE!
  *  The symbol VECT_TAB_SRAM needs to be defined when building the project
  *  if code has been located to RAM and interrupts are used. 
  *  Otherwise the interrupt table located in flash will be used.
  *  See also the <system_*.c> file and how the SystemInit() function updates 
  *  SCB->VTOR register.  
  *  E.g.  SCB->VTOR = 0x20000000;  
  */

  /* TODO - Add your application code here */

	GPIOConfigurePin(RGBLED_BLUE,ioPortOutputPushPull);
	GPIOConfigurePin(RGBLED_GREEN,ioPortOutputPushPull);
	GPIOConfigurePin(RGBLED_RED,ioPortOutputPushPull);

	GPIOConfigurePin(BUTTON_BLUE,ioPortInputFloat);
	GPIOConfigurePin(BUTTON_LEFT,ioPortInputFloat);
	GPIOConfigurePin(BUTTON_RIGHT,ioPortInputFloat);

	//time_t t;
	//srand((unsigned) time(&t));

  /* Infinite loop */
  while (1)
  {
	  int i = rand()%3;

	  switch(i)
	  {
	  case 0:
		  GPIOWrite(RGBLED_GREEN,true);
		  break;
	  case 1:
		  GPIOWrite(RGBLED_RED,true);
		  break;
	  case 2:
		  GPIOWrite(RGBLED_BLUE,true);
		  break;
	  }


for(int i = 0;i<1000000;i++);

GPIOWrite(RGBLED_BLUE,false);
GPIOWrite(RGBLED_GREEN,false);
GPIOWrite(RGBLED_RED,false);


  }
}