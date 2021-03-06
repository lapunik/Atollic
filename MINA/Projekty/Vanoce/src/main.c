/* Includes */
#include "nucleo_core.h"
#include "nucleo_uart.h"
#include "systick_ms.h"
#include "oled_1305.h"
#include "disp_hilevel.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "picture.h"
#if (HSE_VALUE != 8000000)
#error HSE_VALUE must be set to 8MHz - parameters of project (or system_stm32f4xx.c) !!
#endif

/**
 **===========================================================================
 **
 **  Abstract: main program
ik **
 **===========================================================================
 */

bool snih[snih_width][snih_height];
bool strom[strom_width][strom_height];
bool darek_velky[darek_velky_width][darek_velky_height];
bool darek_maly[darek_maly_width][darek_maly_height];


int snow_position = 0;
uint32_t tick = 0;


bool PictureInit(char *array_from,bool *array_to, int width, int height)
{
    uint32_t slovo = 0;
    uint32_t bit = 0;

    // p?eveden? HEX matice o 2048 prvc?ch (ka?d? obsahuje 8bit? -> 16?384 hodnot) do BOOL matice 32*512 (16 384 true nebo false)
    for(int y = 0; y < height ; y++)
    {
        for(int x = 0 ; x < width ; x++)
        {

            *((array_to + y * width) + x) = ((array_from[slovo] & (0x01 << bit)) == 0)?false:true;

            bit++;
            if(bit == 8)
            {
                bit = 0;
                slovo++;
            }

        }
    }

    return true;

}

bool DrawPicture(bool *array,int x0, int y0, int width, int height,int posunX, int posunY, bool transparent)
{

    // okno pro vykreslen?
    for(int y = 0; y < height ; y++)
    {
        for(int x = 0 ; x < width ; x++)
        {
            bool arr = *((array + y * width) + x);

            if((arr)||(!transparent)) // pokud je co kreslit nebo pokud chci kreslit i "b?lou", tak kresli
            {
                DISP_FillRect(x0+x+posunX,y0+y+posunY,1,1,arr);
            }



        }
    }

    return true;

}

void TIM2_IRQHandler()
{
    TIM2->SR &= ~TIM_SR_UIF; // shozen? p??znaku (u SysTick nen? pot?eba)

    tick++;

    if(tick%50==0)
    {
        DISP_Clear();
        DISP_GotoXY(0,0);
        DISP_WriteString("Vesle");
        DISP_FillRect(38,0,1,1,true);
        DISP_FillRect(39,0,1,1,true);
        DISP_FillRect(37,1,1,1,true);
        DISP_FillRect(38,1,1,1,true);
        DISP_GotoXY(1,1);
        DISP_WriteString("Vanoce");
        DISP_FillRect(21,8,1,1,true);
        DISP_FillRect(22,8,1,1,true);
        DISP_FillRect(20,9,1,1,true);
        DISP_FillRect(21,9,1,1,true);

        DrawPicture(&strom[0][0],82,0,strom_width,strom_height,0,0,true);
        DrawPicture(&snih[0][0],0,-32,snih_width,snih_height,0,snow_position,true);

        if(tick>1000)
        {
            DISP_FillRect(106,19,8,8,false);
            DISP_FillRect(107,16,10,10,false);
            DrawPicture(&darek_maly[0][0],103,13,darek_maly_width,darek_maly_height,0,0,true
                    );            ;
        }
        if(tick>2000)
        {
            DISP_FillRect(52,22,27,8,false);
            DISP_FillRect(53,21,27,8,false);
            DrawPicture(&darek_velky[0][0],50,17,darek_velky_width,darek_velky_height,0,0,true);
        }
        if(tick>3000)
        {
            DISP_FillRect(64,14,8,8,false);
            DISP_FillRect(65,11,10,10,false);
            DrawPicture(&darek_maly[0][0],61,8,darek_maly_width,darek_maly_height,0,0,true);
        }
        if(tick>4000)
        {
        tick = 0;
        }


        snow_position++;
        if(snow_position == 32)
        {
            snow_position = 0;
        }
    }

}
int main(void)
{

    // ----- SYSTEM CLOCK --------------------------------------------------------------------------------------

    SystemClock_100MHz();
    SystemCoreClockUpdate();
    InitSystickDefault();

    // ----- SYSTEM CLOCK --------------------------------------------------------------------------------------
    // ----- SYSTEM GPIO ---------------------------------------------------------------------------------------

    Nucleo_SetPinGPIO(GPIOA, 0, input_pullup);
    Nucleo_SetPinGPIO(GPIOA,5, output);

    // ----- SYSTEM CLOCK --------------------------------------------------------------------------------------
    // ----- TIM2 ----------------------------------------------------------------------------------------------

    NVIC_EnableIRQ(TIM2_IRQn);

    // Povolen? hodin sb?rnici:
    if(!(RCC->APB1ENR & RCC_APB1ENR_TIM2EN)) // APB1/APB2/AHB1/ Datasheet str. 15 je sch?ma co je ke kter? sb?rnici p?ipojeno
    {
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
        RCC->APB1RSTR |= RCC_APB1RSTR_TIM2RST;
        RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;
    }

    // Nastaven? timeru:
    TIM2->CR1 = TIM_CR1_DIR; // Dekrementace
    TIM2->CNT &= ~TIM_CNT_CNT; // Nulov?n? counteru
    TIM2->DIER = TIM_DIER_UIE; // Nastaven? generov?n? p?eru?en?

    TIM2->PSC = (GetBusClock(timersClockAPB1)/ 1000000)-1;         // PSC: 1   [us]    =>      1     [MHz]
    TIM2->ARR = (1000) - 1;                                        // ARR: 1   [ms]    =>      1     [kHz]

    TIM2->CR1 |= TIM_CR1_CEN;

    // ----- TIM2 ----------------------------------------------------------------------------------------------
    // ----- OLED ----------------------------------------------------------------------------------------------
    OLED_Init();
    WaitMs(50);
    DISP_Clear();
    DISP_GotoXY(0, 0);
    DISP_SetFont(font_atari_8x8);

    // ----- OLED ----------------------------------------------------------------------------------------------


    PictureInit(darek_maly_byte,&darek_maly[0][0],darek_maly_width,darek_maly_height);
    PictureInit(darek_velky_byte,&darek_velky[0][0],darek_velky_width,darek_velky_height);
    PictureInit(snih_byte,&snih[0][0],snih_width,snih_height);
    PictureInit(strom_byte,&strom[0][0],strom_width,strom_height);

    DISP_Clear();

    while (1)
    {

    }
}
