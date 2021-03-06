/* Includes */
#include "nucleo_core.h"
#include "nucleo_uart.h"
#include "systick_ms.h"
#include "oled_1305.h"
#include "disp_hilevel.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "picture.h"
#if (HSE_VALUE != 8000000)
#error HSE_VALUE must be set to 8MHz - parameters of project (or system_stm32f4xx.c) !!
#endif
#include <math.h>

/**
 **===========================================================================
 **
 **  Abstract: main program
ik **
 **===========================================================================
 */

# define SIZE_ARRAY 7// 16 pismen x 4 sloupce

char data_receive[SIZE_ARRAY];
volatile uint16_t receive_counter = 0;
uint stopWatch = 1;
float stopWatch_hold = 0;
uint32_t tim2_ticks = 0;
bool timetable_active = false;
bool animation = false;
bool obsazenost[8];
bool train_bit[train_width][train_height];
int posun_vlak = -200;
int sleep_mode = 0;
int sleep_counter = 0;

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

            if((((x0+x+posunX)<=255)&&((x0+x+posunX)>=0))&&(((y0+y+posunY)<=31)&&((y0+y+posunY)>=0)))
            {

                if((arr)||(!transparent)) // pokud je co kreslit nebo pokud chci kreslit i "b?lou", tak kresli
                {
                    DISP_FillRect(x0+x+posunX,y0+y+posunY,1,1,arr);
                }

            }
        }

    }
    return true;

}

void DISP_Time(int value,int count)
{
    char new [4] = "0000";
    sprintf(new,"%u",value);

    if(value<10)
    {
        new[1] = new[0];
        new[0] = ' ';
    }

    new[2] = ':';
    for(int i = 0;i<3;i++)
    {
        if(new[i] == '\0')
        {
            new[i] = ' ';
        }
    }
    new[3] = '\0';
    if(count == 2)
    {
        new[2] = '\0';
    }

    DISP_GotoXY(8+(3*count),0);
    DISP_WriteString(new);
}

void NumberToTime(int num)
{
    uint hours = 0;
    uint minutes = 0;
    uint seconds= 0;

    while(num >= (60*60))
    {
        hours++;
        num -= (60*60);
    }
    while(num >= 60)
    {
        minutes++;
        num -= 60;
    }
    seconds = num;

    DISP_Time(hours,0);

    DISP_Time(minutes,1);

    DISP_Time(seconds,2);

}

void TIM2_IRQHandler()
{
    TIM2->SR &= ~TIM_SR_UIF; // shozen? p??znaku (u SysTick nen? pot?eba)

    if(timetable_active)
    {

        animation = true;

        if(stopWatch <= 0) // ud?lej animaci
        {

            // ------ SIMULACE POMALEHO ROZJEZDU A BR??N? ------------------------------
            uint milSec = 0;
            /*
            if(posun_vlak > ((train_width - 128)-20))
            {
                milSec = 80; // 80ms
            }
            else if(posun_vlak > ((train_width - 128)-40))
            {
                milSec = 50; // 50ms
            }
            else if(posun_vlak > ((train_width - 128)-60))
            {
                milSec = 30; // 30ms
            }
            else if((posun_vlak < 40)&&(posun_vlak>30))
            {
                milSec = 50; // 50ms
            }
            else if((posun_vlak < 30)&&(posun_vlak>20))
            {
                milSec = 70; // 70ms
            }
            else if((posun_vlak < 20)&&(posun_vlak>0))
            {
                milSec = 100; // 100ms
            }
            else
            {
                milSec = 20; // 20ms
            }
             */

            milSec = 20;

            TIM2->ARR = (1000*milSec) - 1;

            float time = 0.001*milSec;

            // ------------------------------------------------------------------------

            DISP_Clear();
            DrawPicture(&train_bit[0][0],0,0,train_width,train_height,posun_vlak,-2,false);
            DISP_DrawLine(0,30,127,30,true);
            DISP_DrawLine(0,31,127,31,true);


            posun_vlak++;

            if(stopWatch_hold > 0)
            {
                stopWatch_hold = stopWatch_hold - time;
            }
            if(posun_vlak == 100)
            {
                posun_vlak = -200;
                timetable_active = false;
                tim2_ticks = 0;
                TIM2->ARR = (1000 * 1000) - 1;                                 // ARR: 1   [s]     =>      1     [Hz]
                animation = false;

                if(stopWatch_hold > 0)
                {
                    tim2_ticks = 0;
                    timetable_active = true;
                    stopWatch = (int)(round(stopWatch_hold)-1);
                    stopWatch_hold = 0;
                }

            }

        }
        else if((stopWatch>10799)&&(sleep_mode==0))
        {

            sleep_counter++;
            if(sleep_counter>5)
            {
                sleep_mode = 1;
                sleep_counter = 0;
            }

            DISP_Clear(); ////////////////////////////////////////////////////////////////////////
            DISP_GotoXY(0,0);
            DISP_WriteString("Obsazenost:");
            DISP_GotoXY(0,2);
            DISP_WriteString("BA|BB|BC|KA|KB|L");
            DISP_GotoXY(0,3);
            for(int i = 0;i<6;i++)
            {
                DISP_WriteChar((obsazenost[i+1])?'X':'-');
                if(i!=5)
                {
                    DISP_WriteString(" |");
                }
            }

            stopWatch--;

            if(stopWatch_hold > 0)
            {
                stopWatch_hold = stopWatch_hold - 1;
            }
        }

        else // vypisuj ?as
        {
            sleep_counter++;
            if(sleep_counter>5)
            {
                sleep_mode = 0;
                sleep_counter = 0;
            }

            DISP_Clear();
            DrawPicture(&train_bit[0][0],0,0,train_width,train_height,posun_vlak,-2,false);
            DISP_DrawLine(0,30,127,30,true);
            DISP_DrawLine(0,31,127,31,true);
            NumberToTime(stopWatch);
            stopWatch--;

            if(stopWatch_hold > 0)
            {
                stopWatch_hold = stopWatch_hold - 1;
            }
        }


    }


}

void USART2_IRQHandler() {

    if(USART2->SR & USART_SR_RXNE)
    {

        uint8_t receive =  USART2->DR;

        if(receive_counter<SIZE_ARRAY)
        {
            if((receive != '\n') && (receive != '\r'))
            {
                data_receive[receive_counter] = receive;
                receive_counter++;
            }
        }


        else
        {
            data_receive[receive_counter] = receive;
            receive = '\n';
        }

        if((receive == '\n')||(receive == '\r'))
        {
            uint size_array = ((sizeof(data_receive))/(sizeof(data_receive[0])))-1;
            char my_array[size_array];
            for(int i = 0;i<size_array;i++)
            {
                my_array[i] = data_receive[i];
            }

            if((data_receive[0]=='r')&&(data_receive[1]=='e')&&(data_receive[2]=='s')&&(data_receive[3]=='e')&&(data_receive[4]=='t'))
            {
                NVIC_SystemReset();
            }

            if(animation == false)
            {
                sscanf(my_array, "%d", &stopWatch);

                tim2_ticks = 0;
                timetable_active = true;
                receive_counter = 0;
                for(int i = 0; i<SIZE_ARRAY+2; i++)
                {
                    data_receive[i]= '\0';
                }

            }
            else
            {

                int stH = 0;
                sscanf(my_array, "%d", &stH);
                //sscanf(my_array, "%f", &stopWatch_hold_float);
                // round prijateho cisla
                stopWatch_hold = (float)stH;
                receive_counter = 0;
                for(int i = 0; i<SIZE_ARRAY+2; i++)
                {
                    data_receive[i]= '\0';
                }

            }



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
    // ----- GPIO ----------------------------------------------------------------------------------------------

    NVIC_EnableIRQ(USART2_IRQn);
    Nucleo_StartUart2(38400);


    setvbuf(stdout,NULL,_IONBF,0);
    setvbuf(stdin,NULL,_IONBF,0);

    // ----- UART ----------------------------------------------------------------------------------------------
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
    TIM2->ARR = (1000 * 1000) - 1;                                 // ARR: 1   [s]     =>      1     [Hz]

    TIM2->CR1 |= TIM_CR1_CEN;

    // ----- TIM2 ----------------------------------------------------------------------------------------------
    // ----- OLED ----------------------------------------------------------------------------------------------

    puts("\n\nOLED start: " __DATE__ " " __TIME__);
    printf("CoreClock: %ld\n", SystemCoreClock);

    OLED_Init();
    WaitMs(50);
    DISP_Clear();
    DISP_GotoXY(0, 0);
    DISP_SetFont(font_atari_8x8);

    puts("OLED Init finish");

    // ----- OLED ----------------------------------------------------------------------------------------------
    // ---- VARIABLES ------------------------------------------------------------------------------------------

    //timeBase = _ticks - 1;

    PictureInit(train,&train_bit[0][0],train_width,train_height);


    obsazenost[0]=false;
    obsazenost[1]=false;
    obsazenost[2]=true;
    obsazenost[3]=false;
    obsazenost[4]=true;
    obsazenost[5]=true;
    obsazenost[6]=false;
    obsazenost[7]=true;

    DISP_Clear();
    DISP_GotoXY(0,1);
    DISP_WriteString("Waiting for");
    DISP_GotoXY(2,2);
    DISP_WriteString("information...");

    while (1)
    {
        /*        if (_ticks > timeBase)
        {
        }
         */
    }
}
