#include "mina_shield_mbed.h"

void TIM3_IRQHandler(void)
{
    if(TIM3->SR & TIM_SR_UIF)
    {
        TIM3->SR &= ~TIM_SR_UIF;
        BB_REG(TIM3->CR1, 0) = 0; // TIM_CR1_CEN
        BB_REG(GPIOA->ODR, 5) = 1; // zhasni LED
    }
}

int main(void) {

    //SetClock100MHz(clockSourceHSE);

    //SysTick_Config(SystemCoreClock / 1000);
    //SysTick_Config(100000000 / 1000);
    //SysTick_Config(1000000/1000);

    //RCC -> CFGR |= RCC_CFGR_HPRE_DIV16; // pro 1MHz

    //GPIOConfigurePin(BOARD_LED,ioPortOutputPushPull);

    NVIC_EnableIRQ(TIM3_IRQn);

    SystemCoreClockUpdate();

    GPIOConfigurePin(BOARD_LED,ioPortOutputPushPull);
    GPIOConfigurePin(BUTTON_BLUE,ioPortInputFloat);

    if (!(RCC->APB1ENR & RCC_APB1ENR_TIM3EN))
    {
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    RCC->APB1RSTR |= RCC_APB1RSTR_TIM3RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM3RST;
    }

    if (!(RCC->APB2ENR & RCC_APB2ENR_SYSCFGEN))
    {
        RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
        RCC->APB2RSTR |= RCC_APB2RSTR_SYSCFGRST;
        RCC->APB2RSTR &= ~RCC_APB2RSTR_SYSCFGRST;
    }

    SYSCFG->EXTICR[3] &= SYSCFG_EXTICR4_EXTI13;
    SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;
    EXTI->EMR |= EXTI_EMR_MR13; // PC13
    EXTI->RTSR |= EXTI_RTSR_TR13; // pust = hrana L-H

    uint32_t tm = 0;
    uint32_t tmBut = 0;
    bool lastBut = true;
    uint32_t _ticks = 0;

    while (1)
    {
        for (int i = 0; i < 2000; i++) // pri 16MHz
            ;
        _ticks++;
        if (_ticks >= tm)
        {
            tm = _ticks + 50;
            BB_REG(GPIOA->ODR, 5) ^= 1; // OnBoard LED
        }
        if (_ticks >= tmBut)
        {
            tmBut = _ticks + 5;
            bool bb = GPIORead(BUTTON_BLUE);
            if (lastBut != bb)
            {
                lastBut = bb;
                if (bb) // L-H hrana ?
                {
                    //SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;
                    //SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
                    //SCB->SCR |=  SCB_SCR_SLEEPDEEP_Msk;

                    BB_REG(GPIOA->ODR, 5) = 0; // zhasni LED

                    TIM3->CNT = 1; // fresh start
                    TIM3->CR1 &= ~TIM_CR1_DIR; // Dekrementace
                    TIM3->CNT &= ~TIM_CNT_CNT; // Nulování counteru
                    TIM3->PSC = (SystemCoreClock/ 1000)-1;         // 1 [ms]    =>      1     [MHz]
                    TIM3->ARR = (5000) - 1;                    // 1 [s] * 5 = 5 [s]
                    TIM3->DIER = TIM_DIER_UIE; // Nastavení generování pøerušení
                    TIM3->CR1 |= TIM_CR1_CEN;

                    SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;
                    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;

                    asm("WFI");

                }
            }
        }
    }
}
