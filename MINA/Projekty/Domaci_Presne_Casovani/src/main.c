#include "mina_shield_mbed.h"

volatile uint32_t ticks = 0;

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Pøerušení od SysTicku:
void SysTick_Handler(void)
{
    ticks++;
}
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Pøerušení od TIM2:
void TIM2_IRQHandler(void)
{
    TIM2->SR &= ~TIM_SR_UIF; // shození pøíznaku (u SysTick není potøeba)

    GPIOToggle(RGBLED_BLUE); // Blikni
}

int main(void)
{
    // --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Výbìr zdroje hodin:
    // Hodiny defalutnì nastaveny na 16MHz (SystemCoreClock)
    SetClock100MHz(clockSourceHSE); // nastav hodiny na 100MHz

    SystemCoreClockUpdate(); // Reaguj na to že jsem nastavil hodiny

    // --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Nastavení SysTicku: (popis v Programming manuálu)
    SysTick_Config(GetBusClock(timersClockAPB1) / 1000);  // SysTick nastaven na 1   [ms]    (každou 1ms bude vykonávat pøerušení, kde se inkrementuje "ticks")
    //SysTick_Config(GetBusClock(timersClockAPB1) / 100); // SysTick nastaven na 10  [ms]
    //SysTick_Config(GetBusClock(timersClockAPB1) / 10);  // SysTick nastaven na 100 [ms]
    //SysTick_Config(GetBusClock(timersClockAPB1));       // SysTick nastaven na 1   [s]

    // --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Povolení pøerušení TIM2:
    NVIC_EnableIRQ(TIM2_IRQn); // SysTick obdobnì nastaven ve funkci SysTick_Config

    // --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Povolení hodin sbìrnici:
    if(!(RCC->APB1ENR & RCC_APB1ENR_TIM2EN)) // APB1/APB2/AHB1/ Datasheet str. 15 je schéma co je ke které sbìrnici pøipojeno
    {
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
        RCC->APB1RSTR |= RCC_APB1RSTR_TIM2RST;
        RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;
    }

    // --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Nastavení timeru:
    TIM2->CR1 = TIM_CR1_DIR; // Dekrementace
    TIM2->CNT &= ~TIM_CNT_CNT; // Nulování counteru
    TIM2->DIER = TIM_DIER_UIE; // Nastavení generování pøerušení

    // --- PSC => 1us ----------------------------------------------------------------------------------------------|
    //                                                                                                              |
    // TIM2->PSC = (GetBusClock(timersClockAPB1)/ 1000000)-1;         // PSC: 1   [us]    =>      1     [MHz]       |
    // TIM2->ARR = (1000 * 10000) - 1;                                // ARR: 10  [s]     =>      1     [Hz]        |
    //                                                                                                              |
       TIM2->PSC = (GetBusClock(timersClockAPB1)/ 1000000)-1;         // PSC: 1   [us]    =>      1     [MHz]       |
       TIM2->ARR = 5*(1000 * 1000) - 1;                                 // ARR: 1   [s]     =>      1     [Hz]        |
    //                                                                                                              |
    // TIM2->PSC = (GetBusClock(timersClockAPB1)/ 1000000)-1;         // PSC: 1   [us]    =>      1     [MHz]       |
    // TIM2->ARR = (1000 * 100) - 1;                                  // ARR: 100 [ms]    =>      10    [Hz]        |
    //                                                                                                              |
    // TIM2->PSC = (GetBusClock(timersClockAPB1)/ 1000000)-1;         // PSC: 1   [us]    =>      1     [MHz]       |
    // TIM2->ARR = (1000 * 10) - 1;                                   // ARR: 10  [ms]    =>      100   [Hz]        |
    //                                                                                                              |
    // TIM2->PSC = (GetBusClock(timersClockAPB1)/ 1000000)-1;         // PSC: 1   [us]    =>      1     [MHz]       |
    // TIM2->ARR = 1000 - 1;                                          // ARR: 1   [ms]    =>      1000  [Hz]        |
    //                                                                                                              |
    //--------------------------------------------------------------------------------------------------------------|
    //                                                                                                              |
    //          ! ! ! POZOR: Pro 100MHz na vstupu nelze nastavit PSC na 1ms, pøeteèení registru ! ! !               |
    //                                                                                                              |
    // --- PSC => 1ms ----------------------------------------------------------------------------------------------|
    //                                                                                                              |
    // TIM2->PSC = (GetBusClock(timersClockAPB1)/ 1000)-1;            // PSC: 1   [ms]    =>      1     [kHz]       |
    // TIM2->ARR = 10 - 1;                                            // ARR: 10  [ms]    =>      100   [Hz]        |
    //                                                                                                              |
    // TIM2->PSC = (GetBusClock(timersClockAPB1)/ 1000)-1;            // PSC: 1   [ms]    =>      1     [kHz]       |
    // TIM2->ARR = 100 - 1;                                           // ARR: 100 [ms]    =>      10    [Hz]        |
    //                                                                                                              |
    // TIM2->PSC = (GetBusClock(timersClockAPB1)/ 1000)-1;            // PSC: 1   [ms]    =>      1     [kHz]       |
    // TIM2->ARR = 1000 - 1;                                          // ARR: 1   [s]     =>      1     [Hz]        |
    //                                                                                                              |
    // TIM2->PSC = (GetBusClock(timersClockAPB1)/ 1000)-1;            // PSC: 1   [ms]    =>      1     [kHz]       |
    // TIM2->ARR = (1000 * 10) - 1;                                   // ARR: 10  [s]     =>      0.1   [Hz]        |
    // -------------------------------------------------------------------------------------------------------------|

    TIM2->CR1 |= TIM_CR1_CEN;

    // --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Konfigurace GPIO:
    GPIOConfigurePin(RGBLED_BLUE, ioPortOutputPushPull);
int i = 0;
    while(1)
    {
i++;
    }
}

