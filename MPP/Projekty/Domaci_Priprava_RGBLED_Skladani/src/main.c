/*
******************************************************************************
File:     main.c
Info:     Generated by Atollic TrueSTUDIO(R) 9.3.0   2019-02-27

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

#include "stm_core.h"
#include "mpp_shield.h"

int main(void)
{

	GPIOConfigurePin(RGBLED_GREEN, ioPortOutputPushPull);
	GPIOConfigurePin(RGBLED_RED, ioPortOutputPushPull);
	GPIOConfigurePin(RGBLED_BLUE, ioPortOutputPushPull);
	GPIOConfigurePin(BUTTON_LEFT,ioPortInputFloat);
	GPIOConfigurePin(BUTTON_RIGHT,ioPortInputFloat);
	GPIOConfigurePin(BUTTON_BLUE,ioPortInputFloat);

	// pokud se pt�m jestli je na vstupu jedni�ka nebo nula IDR, moje funkce mi to neum� ��ct

  while (1)
  {

		if(!GPIORead(BUTTON_BLUE))
		{
		    GPIOToggle(RGBLED_BLUE);

		    while(!GPIORead(BUTTON_BLUE))
		    {

		    }
		}
		if(!GPIORead(BUTTON_RIGHT))
		{
		    GPIOToggle(RGBLED_RED);

		    while(!GPIORead(BUTTON_RIGHT))
		    {

		    }
		}
		if(!GPIORead(BUTTON_LEFT))
		{
		    GPIOToggle(RGBLED_GREEN);

		    while(!GPIORead(BUTTON_LEFT))
		    {

		    }
		}


  }
}