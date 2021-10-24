/*
 * oled_1305.c
 *
 *  Created on: 3. 12. 2016
 *      Author: weiss_000
 */

#include "oled_1305.h"
#include "oled_1305_commands.h"

void OLED_Write8(uint8_t b);
void OLED_WriteCmd(uint8_t cmd);
void OLED_SetPage(int pg);
void OLED_SetAllRow(void);

void OLEDInit_HW(void);
void OLEDInit_SW(void);

#define OLED_DC_HIGH GPIO_WRITE(GPIOB, 4, 1)
#define OLED_DC_LOW GPIO_WRITE(GPIOB, 4, 0)

#define OLED_CS_HIGH GPIO_WRITE(GPIOC, 0, 1)
#define OLED_CS_LOW GPIO_WRITE(GPIOC, 0, 0)

#define OLED_EN_HIGH GPIO_WRITE(GPIOB, 3, 1)
#define OLED_EN_LOW GPIO_WRITE(GPIOB, 3, 0)

#define OLED_RESET_HIGH GPIO_WRITE(GPIOB, 5, 1)
#define OLED_RESET_LOW GPIO_WRITE(GPIOB, 5, 0)

#ifndef SPI_IS_BUSY
#define SPI_IS_BUSY(SPIx) (((SPIx)->SR & (SPI_SR_TXE | SPI_SR_RXNE)) == 0 || ((SPIx)->SR & SPI_SR_BSY))
#define SPI_WAIT(SPIx)            while (SPI_IS_BUSY(SPIx))
#endif

void OLEDInit_HW(void)
{
  if (!(RCC->APB2ENR & RCC_APB2ENR_SPI1EN))
  {
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;
  }

  SPI1->CR1 = SPI_CR1_BR_1 | SPI_CR1_BR_0;      // 011 = clk/16 - z APB2 (100MHz), LCD max. 10MHz
  SPI1->CR1 |= SPI_CR1_MSTR;
  SPI1->CR1 |= SPI_CR1_SSI | SPI_CR1_SSM;
  SPI1->CR1 |= SPI_CR1_CPHA | SPI_CR1_CPOL; // viz. RM0383 pg. 555/836 (rev 1)
  SPI1->CR2 = 0;

  SPI1->CR1 |= SPI_CR1_SPE;

  Nucleo_SetPinGPIO(GPIOA, 5, alter);   // D13 - SPI1 - SCK, OLED - SCK
  Nucleo_SetPinAFGPIO(GPIOA, 5, 5);
//  Nucleo_SetPinGPIO(GPIOA, 6, alter);   // SPI1 - MISO, LCD - SDO
//  Nucleo_SetPinAFGPIO(GPIOA, 6, 5);
  Nucleo_SetPinGPIO(GPIOA, 7, alter);   // D11 - SPI1 - MOSI, OLED - MOSI
  Nucleo_SetPinAFGPIO(GPIOA, 7, 5);

  Nucleo_SetPinGPIO(GPIOC, 0, output); // D10 - OLED - CS
  GPIO_WRITE(GPIOC, 0, 1);
  Nucleo_SetPinGPIO(GPIOB, 5, output);  // D6 - OLED - RES
  GPIO_WRITE(GPIOB, 5, 1);
  Nucleo_SetPinGPIO(GPIOB, 4, output);  // D7 - OLED - D/C
  GPIO_WRITE(GPIOB, 4, 1);
}

static volatile int xPtr = 0, yPtr = 0;
static volatile bool bbUseDMA = true;

static volatile uint8_t oledBuffer[OLED_HEIGHT_BYTES][OLED_WIDTH_BYTES];
static volatile uint8_t oledSendBuffer[OLED_HEIGHT_BYTES][OLED_WIDTH_BYTES];

static volatile uint32_t refreshAutoPeriod = 0;
static volatile bool refreshDMA = true;
static volatile int refreshDMAStageSPI = -1;   // first is MEM, second is SPI lines

static volatile bool refreshInProgess = false;

void OLED_Write8(uint8_t b)
{
  SPI1->DR = b;
//  while(!(SPI1->SR & SPI_SR_BSY))  // wait for finish
  SPI_WAIT(SPI1)
    ;
}

void OLED_WriteCmd(uint8_t cmd)
{
  OLED_DC_LOW;
  OLED_CS_LOW;

  OLED_Write8(cmd);

  OLED_CS_HIGH;
}

void OLEDInit_SW(void)
{
  uint32_t tm = 0;

  OLED_RESET_LOW;
  for(tm = 0; tm < 50000; tm++)
    asm("nop");

  OLED_RESET_HIGH;
  for(tm = 0; tm < 10000; tm++)
    asm("nop");

  OLED_DC_LOW;
  OLED_CS_LOW;

  OLED_Write8(SSD1305_MEMORYMODE);
  OLED_Write8(0x00);

  OLED_CS_HIGH;

  OLED_WriteCmd(SSD1305_DISPLAYON);
}

void OLED_SetPage(int pg)
{
  OLED_DC_LOW;
  OLED_CS_LOW;

  OLED_Write8(SSD1305_SETPAGEADDR);
  OLED_Write8(pg % 4);
  OLED_Write8(pg % 4);

  OLED_CS_HIGH;
}

void OLED_SetAllRow(void)
{
  OLED_DC_LOW;
  OLED_CS_LOW;

  OLED_Write8(SSD1305_SETCOLADDR);
  OLED_Write8(0);
  OLED_Write8(OLED_WIDTH_BYTES);

  OLED_CS_HIGH;
}

void OLED_Refresh(void)
{
  if (refreshInProgess)
  {
    //TODO show error
    return;
  }

  refreshInProgess = true;

  if (refreshDMA)
  {
    DMA2_Stream3->CR &= ~DMA_SxCR_EN;

    DMA2->LIFCR |= (DMA_LIFCR_CTEIF3 | DMA_LIFCR_CHTIF3 | DMA_LIFCR_CTCIF3 | DMA_LIFCR_CDMEIF3);

    /*
    DMA1_Channel3->CCR = DMA_CCR3_MEM2MEM
      | DMA_CCR3_MSIZE_0   // 01 = 16b
      | DMA_CCR3_PSIZE_0   // 01 = 16b
      | DMA_CCR3_MINC
      | DMA_CCR3_PINC      // vypada to, ze se prenasi z P do M
      | DMA_CCR3_DIR
        // PL = 00 = low priority
      | DMA_CCR3_TCIE
      ;
    */
    DMA2_Stream3->CR = 0
       | DMA_SxCR_CHSEL_0 | DMA_SxCR_CHSEL_1  // 011 = channel 3 in stream 3
       | DMA_SxCR_DIR_1  // 10 = mem to mem = DMA_SxPAR to DMA_SxM0AR
       | DMA_SxCR_MINC
       | DMA_SxCR_PINC
       | DMA_SxCR_TCIE   // irq "complete" fire          DMA_CCR3_MINC
       ;

    DMA2_Stream3->PAR = (uint32_t)oledBuffer;       // SRC
    DMA2_Stream3->M0AR = (uint32_t)oledSendBuffer;          // DEST

    DMA2_Stream3->NDTR = OLED_WIDTH_BYTES * OLED_HEIGHT_BYTES;   // nutno plnit vzdy, po prenosu ukazuje pocet zbyvajicich, tedy 0

    refreshDMAStageSPI = -1;                            // start stage: MEM
    DMA2_Stream3->CR |= DMA_SxCR_EN;                    // go
  }
  else
  {
    uint y, x;

    for(y = 0; y < OLED_HEIGHT_BYTES; y++)
      for(x = 0; x < OLED_WIDTH_BYTES; x++)
        oledSendBuffer[y][x] = oledBuffer[y][x];

    for(y = 0; y < OLED_HEIGHT_BYTES; y++)
    {
      OLED_SetPage(y);
      OLED_SetAllRow();

      OLED_DC_HIGH;
      OLED_CS_LOW;

      for(x = 0; x < OLED_WIDTH_BYTES; x++)
        OLED_Write8(oledSendBuffer[y][x]);

      OLED_CS_HIGH;
    }

    refreshInProgess = false;
  }
}

static void OLED_DrawPixel(uint8_t x, uint8_t y, bool hilite)
{
  if ((x < OLED_WIDTH_PIX) && (y < OLED_HEIGHT_PIX))
  {
    if (hilite)
      oledBuffer[y / 8][x] |= 1 << (y & 0x07);   // snad rychlejsi nez y % 8
    else
      oledBuffer[y / 8][x] &= ~(1 << (y & 0x07));
  }
}

static void OLED_SetByte(int x, int row, uint8_t val)
{
  oledBuffer[row][x] = val;
}

//static void (* my_SetBytes)(int x, int row, uint8_t *data, int len) = NULL;

// nechce se mi to cele includovat ... #include "disp_hilevel.h"
void SetHiLevelDisp(int pixWidth, int pixHeight,
    void *ptrDrawPixel, void *ptrSetByte, void *ptrSetBytes);

bool OLED_Init(void)
{
  bool bbRet = true;
  uint32_t apb = GetTimerClock(REFRESH_TIMER);

  OLEDInit_HW();
  OLEDInit_SW();

  SetHiLevelDisp(OLED_WIDTH_PIX, OLED_HEIGHT_PIX,
      OLED_DrawPixel, OLED_SetByte, NULL);

  bbUseDMA = false;

  if (apb == 0)
    bbRet = false;
  else
  {
    TIM_TypeDef *timPtr = NULL;
    IRQn_Type irqN = 0;
    switch(REFRESH_TIMER)
    {
      case 3:
        if (!(RCC->APB1ENR & RCC_APB1ENR_TIM3EN))
        {
          RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
          RCC->APB1RSTR |= RCC_APB1RSTR_TIM3RST;
          RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM3RST;
        }

        timPtr = TIM3;
        irqN = TIM3_IRQn;
        break;
        //TODO maji jine citace jine registry ?
      default:
        return false;
    }

    timPtr->CR1 = TIM_CR1_URS;
    timPtr->CR2 = 0;
    //    TIM3->EGR = TIM_EGR_UG;

    timPtr->PSC = apb / 1000000 - 1;   // 1us = 1000kHz
    timPtr->ARR = 1000;             // reload, tedy zde 1ms

    timPtr->CR1 |= TIM_CR1_CEN;

    if (irqN > 0)
    {
      timPtr->DIER |= TIM_DIER_UIE;
      NVIC_EnableIRQ(irqN);

      if (!(RCC->AHB1ENR & RCC_AHB1ENR_DMA2EN))
      {
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
        RCC->AHB1RSTR |= RCC_AHB1RSTR_DMA2RST;
        RCC->AHB1RSTR &= ~RCC_AHB1RSTR_DMA2RST;
      }

      NVIC_EnableIRQ(DMA2_Stream3_IRQn);

      bbUseDMA = true;
    }
  }

  return bbRet;
}

#if (REFRESH_TIMER == 3)
void TIM3_IRQHandler(void)
{
  TIM3->SR &= ~TIM_SR_UIF;  // viz. 15.4.5

  if (bbUseDMA)    // any setted ?
    OLED_Refresh();
}
//#elif
#else
//TODO  reagovat na to, ze to chybi
#error Invalid REFRESH_TIMER settings
#endif

void DMA2_Stream3_IRQHandler(void)
{
  if (DMA2->LISR & DMA_LISR_TCIF3)
  {
    DMA2->LIFCR |= DMA_LIFCR_CTCIF3;

    if (refreshDMAStageSPI >= 0)      // nebude se delat pro M2M, ktery je -1
    {
      while(SPI1->SR & SPI_SR_BSY)    // dokud se jeste odesila
        ;

      OLED_CS_HIGH;   // musim cekat na doodeslani (viz. Figure 205. Transmission using DMA - pg.572/836 RM F411)
                      // i kdyz posilam 132 bajtu, ale zobrazuje se pouze 128
    }

    refreshDMAStageSPI++;
    if (refreshDMAStageSPI > 3)
    {
      DMA2_Stream3->CR &= ~(DMA_SxCR_EN | DMA_SxCR_TCIE);   // stop and dis. irq

      SPI1->CR2 &= ~SPI_CR2_TXDMAEN;
      refreshInProgess = false;
    }
    else
    {
      DMA2_Stream3->CR &= ~DMA_SxCR_EN;
      DMA2->LIFCR |= (DMA_LIFCR_CTEIF3 | DMA_LIFCR_CHTIF3 | DMA_LIFCR_CTCIF3 | DMA_LIFCR_CDMEIF3);

      SPI1->CR2 &= ~SPI_CR2_TXDMAEN;

      OLED_SetPage(refreshDMAStageSPI);
      OLED_SetAllRow();

      DMA2_Stream3->CR = 0
        | DMA_SxCR_CHSEL_0 | DMA_SxCR_CHSEL_1  // 011 = channel 3 in stream 3
        | DMA_SxCR_DIR_0  // 01 = mem to peripheral = DMA_SxM0AR to DMA_SxPAR
        | DMA_SxCR_MINC
        | DMA_SxCR_TCIE   // irq "complete" fire          DMA_CCR3_MINC
        ;

      DMA2_Stream3->PAR = (uint32_t)&(SPI1->DR);      // DEST
      DMA2_Stream3->M0AR = (uint32_t)&(oledSendBuffer[refreshDMAStageSPI][0]);       // SRC

      DMA2_Stream3->NDTR = OLED_WIDTH_BYTES;   // nutno plnit vzdy, po prenosu ukazuje pocet zbyvajicich, tedy 0

      SPI1->CR2 |= SPI_CR2_TXDMAEN;

      OLED_DC_HIGH;
      OLED_CS_LOW;

      DMA2_Stream3->CR |= DMA_SxCR_EN;        // go
    }
  }
}
