
#include "mina_shield_mbed.h"
#include "mbed_shield_lcd.h"
#include <stdio.h>
#include <string.h>

// Pro výbìr hardware/software floating point: Project\Properties\C/C+Build\Settings\C Compiler(C Linker)\Floating point\Software/Hardware implementation

#define pixelWidth 128
#define pixelHeight 32
#define tholdCenter 4

volatile uint32_t ticks = 0;

void SysTick_Handler(void) {
	ticks++;
}

int Mandelbrot_Measure()
	{

	float centerX = 1.5;
	float centerY = 1;
    float zoomX =  1;
    float zoomY = 1;

	GPIOWrite(RGBLED_BLUE,false);

	uint32_t time = ticks;

		for (int yy = 0; yy < pixelHeight; yy++)
		      {
		        for (int xx = 0; xx < pixelWidth; xx++)
		        {
		          float x0 = xx;
		          float y0 = yy;
		          x0 = x0 / (pixelWidth / 2 * zoomX) - centerX;
		          y0 = y0 / (pixelHeight / 2 * zoomY) - centerY;

		          float x = 0;
		          float y = 0;
		          float xtemp = 0;

		          int iter = 0;

		          while (((x * x + y * y) < 4) && (iter < 9))
		          {
		            xtemp = x * x - y * y + x0;
		            y = 2 * x * y + y0;
		            x = xtemp;
		            iter++;
		          }
		        }
		      }
		GPIOWrite(RGBLED_BLUE,true);
		return ticks-time;

	}

void Draw_Mandelbrot(float centerX,float centerY,float zoomX,float zoomY)
	{

   // MBED_LCD_FillRect(33, 0, 128, 32, false);
   // MBED_LCD_FillRect(0, 9, 128, 32, false);

   // MBED_LCD_VideoRam2LCD();

		for (int yy = 0; yy < pixelHeight; yy++)
		      {
		        for (int xx = 0; xx < pixelWidth; xx++)
		        {
		          float x0 = xx;
		          float y0 = yy;
		          x0 = x0 / (pixelWidth / 2 * zoomX) - centerX;
		          y0 = y0 / (pixelHeight / 2 * zoomY) - centerY;
		          // z = z * z + c

		          float x = 0;
		          float y = 0;
		          float xtemp = 0;

		          int iter = 0;

		          while (((x * x + y * y) < 4) && (iter < 9))
		          {
		            xtemp = x * x - y * y + x0;
		            y = 2 * x * y + y0;
		            x = xtemp;
		            iter++;
		          }

		          float z = x * x + y * y;

                  if((xx < 33)&&((yy < 9)))
                  {

                  }
                  else
                  {

    		          if (z < tholdCenter)
    		          MBED_LCD_PutPixel(xx, yy, true);
    		          else
    		          MBED_LCD_PutPixel(xx, yy, false);


                  }
		        }
		      }
		MBED_LCD_VideoRam2LCD();


	}

void Draw_String(int cislo)
{

   // MBED_LCD_FillRect(0,0,128,32,false);

	char s[20];

	sprintf(s,"%d",cislo);

    MBED_LCD_WriteStringXY(s,0,0);
    MBED_LCD_VideoRam2LCD();

}

int main(void) {


	////////// CON ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SystemCoreClockUpdate();

	SetClock100MHz(clockSourceHSE);
 // SetClockHSI();
	SysTick_Config(100000000 / 1000);
//	SysTick_Config(SystemCoreClock / 1000);

	GPIOConfigurePin(JOY_LEFT, ioPortInputFloat);

	GPIOConfigurePin(JOY_RIGHT, ioPortInputFloat);

	GPIOConfigurePin(JOY_UP, ioPortInputFloat);

	GPIOConfigurePin(JOY_DOWN, ioPortInputFloat);

	GPIOConfigurePin(JOY_CENTER, ioPortInputFloat);

	GPIOConfigurePin(RGBLED_BLUE, ioPortOutputPushPull);

	////////// CON ///// LCD /////////////////////////////////////////////////////////////////////////////////////////////////////
	if (!MBED_LCD_init()) {
		while (1)
			;
	}

	MBED_LCD_InitVideoRam(0x00);    // fill content with 0 = clear memory buffer

	////////// LCD ///// POT /////////////////////////////////////////////////////////////////////////////////////////////////////
	GPIOConfigurePin(POT_LEFT,ioPortAnalog);
	GPIOConfigurePin(POT_RIGHT,ioPortAnalog);

	  if (!(RCC->APB2ENR & RCC_APB2ENR_ADC1EN))
	  {
	  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	  RCC->APB2RSTR |= RCC_APB2RSTR_ADCRST; // ADCRST je reset bit pro všechny ADC
	  RCC->APB2RSTR &= ~RCC_APB2RSTR_ADCRST; //
	  }

	  ADC1->CR1 = 0 | ADC_CR1_SCAN; // povolení práce s SQR kanály
	  // RES defalutnì nastaven na 00 (12 bitù), takže do toho se nemontuju
	  ADC1->CR2 = 0; // ALIGN = 0 (zarovnání vpravo)
	  // ADON (zapnutí provedeme až uplnì nakonec)
	  ADC1->SMPR1 = 0; // urèení doby pøevodu, chceme pouze pro kanál 0 takže SMPR2
	  ADC1->SMPR2 = ADC_SMPR2_SMP0_1; // 010, pro kanál nula = 28 cyklu


	  ADC1->SQR1 = 0; // L = 0000 = 1 konverze
	  ADC1->SQR2 = 0;
	  ADC1->SQR3 = 0; // SQ1 = 00000 = AD kanal 0
	  ADC->CCR = 0; // TSVREFE = 0 (neni teplotní sensor, neni interní reference), ADCPRE = 0 (APB2 / 2)
	  // CCR spoleèný pro všechny, proto nemá èíslo
	  ADC1->CR2 |= ADC_CR2_ADON; // zapnuti AD

	////////// POT ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

     	 int namereny_cas = 0;

		 float centerX = 1.5;
		 float centerY = 1;
         float zoomX =  1;
         float zoomY = 1;

		 uint32_t pomocna = 0;

	     	GPIOWrite(RGBLED_BLUE,true);

		    MBED_LCD_FillRect(0,0,128,32,false);
		    MBED_LCD_VideoRam2LCD();

		    bool hse = true;
	while (1) {


					if (GPIORead(JOY_LEFT))
					{

						centerX -= 0.1;

					}
					else if (GPIORead(JOY_RIGHT))
					{

						centerX += 0.1;

					}
					else if (GPIORead(JOY_UP))
					{

						centerY -= 0.1;

					}
					else if (GPIORead(JOY_DOWN))
					{

						centerY += 0.1;

					}
					else if (GPIORead(JOY_CENTER))
					{

						if(hse)
						{
						  SetClockHSI();
					      SysTick_Config(SystemCoreClock / 1000);
					      hse = false;
						}
						else
						{
							SetClock100MHz(clockSourceHSE);
							SysTick_Config(100000000 / 1000);
							hse = true;
						}


						while(GPIORead(JOY_CENTER));

					    MBED_LCD_FillRect(0,0,128,32,false);
					    MBED_LCD_VideoRam2LCD();

					}



		for(int i = 0;i<2;i++){

				  ADC1->SQR3 = (!i)?0:1; // zmìna kanálu na AD0 = PA0, AD1 = PA1

				  ADC1->CR2 |= ADC_CR2_SWSTART; // spusteni prevodu

				  while(!(ADC1->SR & ADC_SR_EOC));

				  pomocna = ADC1->DR;

				  if(!i)
				  {
					  zoomX = ((pomocna*(float)3)/(float)4096)+0.25;
				  }
				  else
				  {
				      zoomY = ((pomocna*(float)2)/(float)4096)+0.5;
				  }

		}

	 namereny_cas = Mandelbrot_Measure();

	 Draw_String(namereny_cas);

	 Draw_Mandelbrot(centerX,centerY,zoomX,zoomY);



	}
}
