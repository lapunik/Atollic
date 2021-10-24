/*
 * uart_ring.c
 *
 *  Created on: 5. 5. 2016
 *      Author: weiss
 */

#include "uart_ring.h"

sUartRing UartRingCreate(void *bufPtr_W, uint32_t bufSize_W, void *bufPtr_R, uint32_t bufSize_R)
{
  sUartRing desc;

  desc.IsEmpty_W = true;
  desc.IsOver_W = false;
  desc.BlockingMode_W = false;
  desc.bufWr._buf = (uint8_t *)bufPtr_W;
  desc.bufWr._size = bufSize_W;
  desc.bufWr._rdPtr = desc.bufWr._wrPtr = 0;
  desc._lowLevelSend = desc._lowLevelDisIRQ_W = desc._lowLevelEnIRQ_W = NULL;

  desc.IsEmpty_R = true;
  desc.IsOver_R = false;
  desc.BlockingMode_R = false;
  desc.bufRd._buf = (uint8_t *)bufPtr_R;
  desc.bufRd._size = bufSize_R;
  desc.bufRd._rdPtr = desc.bufRd._wrPtr = 0;
  desc._lowLevelDisIRQ_R = desc._lowLevelEnIRQ_R = NULL;

  return desc;
}

void UartRingInitFn_W(sUartRing *pRing, void *fnEnIRQ_W, void *fnDisIRQ_W, void *fnSend)
{
  pRing->_lowLevelEnIRQ_W = fnEnIRQ_W;
  pRing->_lowLevelDisIRQ_W = fnDisIRQ_W;
  pRing->_lowLevelSend = fnSend;

  return;
}

void UartRingInitFn_R(sUartRing *pRing, void *fnEnIRQ_R, void *fnDisIRQ_R)
{
  pRing->_lowLevelEnIRQ_R = fnEnIRQ_R;
  pRing->_lowLevelDisIRQ_R = fnDisIRQ_R;

  return;
}

bool UartRingIsEmpty_W(sUartRing *pRing)
{
  return pRing->IsEmpty_W;
}

bool UartRingIsOver_W(sUartRing *pRing)
{
  return pRing->IsOver_W;
}

bool UartRingSend(sUartRing *pRing, uint8_t b)
{
  if ((pRing == NULL) || (pRing->bufWr._size == 0))
    return false;

  if (pRing->IsOver_W)
    return false;

  pRing->_lowLevelDisIRQ_W();

  if (pRing->IsEmpty_W)         // test mozna zbytecny, vzdycky to nakonec nebude prazdne
    pRing->IsEmpty_W = false;

  pRing->bufWr._buf[pRing->bufWr._wrPtr] = b;

  pRing->bufWr._wrPtr++;
  if (pRing->bufWr._wrPtr >= pRing->bufWr._size)
    pRing->bufWr._wrPtr = 0;

  if (pRing->bufWr._wrPtr == pRing->bufWr._rdPtr)  // sesly se ? dal se nesmi zapisovat
  {
    if (pRing->BlockingMode_W)           // povolene blokovani pri odeslani ?
    {
      pRing->_lowLevelEnIRQ_W();  // enable TX interrupt, musim povolit, jinak by to nevysilalo

      while(pRing->bufWr._rdPtr == pRing->bufWr._wrPtr)   // tak cekej, az se to uvolni = pohne se RD
        ;

      return true;
    }
    else
      pRing->IsOver_W = true;
  }

  pRing->_lowLevelEnIRQ_W();  // enable TX interrupt, kazdopadne, at jiz pred tim bylo

  return true;
}

void UartRingProcessIRQ_W(sUartRing *pRing)
{
  if (pRing->IsEmpty_W)                // jak to ze to je prazdne ?
  {     // vypnout IRQ pro jistotu, i kdyz uz melo byt vypnute
    pRing->_lowLevelDisIRQ_W();          // disable TX interrupt
  }
  else
  {
    pRing->_lowLevelSend(pRing->bufWr._buf[pRing->bufWr._rdPtr]);

    pRing->bufWr._rdPtr++;
    if (pRing->bufWr._rdPtr >= pRing->bufWr._size)
      pRing->bufWr._rdPtr = 0;

    if (pRing->bufWr._rdPtr == pRing->bufWr._wrPtr)   // dohnal jsem zapsana data ?
    {
      pRing->IsEmpty_W = true;
      pRing->_lowLevelDisIRQ_W();          // disable TX interrupt
    }
  }
}

void UartRingDiscard_W(sUartRing *pRing)
{
//  pRing->bufWr._rdPtr = pRing->bufWr._wrPtr = 0;
//  pRing->IsEmpty_W = pRing->IsOver_W = false;
  while(!pRing->IsEmpty_W)    // cekej na doodvysilani
    ;
}

bool UartRingRecv(sUartRing *pRing, uint8_t *pVal)
{
  if ((pRing == NULL) || (pRing->bufRd._size == 0))
    return false;

  if (pRing->IsEmpty_R)
    return false;

  *pVal = pRing->bufRd._buf[pRing->bufRd._rdPtr];

  pRing->bufRd._rdPtr++;
  if (pRing->bufRd._rdPtr >= pRing->bufRd._size)
    pRing->bufRd._rdPtr = 0;

  if (pRing->bufRd._rdPtr == pRing->bufRd._wrPtr) // ukazuji oba na prvni volne misto ?
  {
    pRing->IsEmpty_R = true;
    // asi neni potreba nic dalsiho delat, hlavne ne blokovat INTy
  }

  return true;
}

bool UartRingIsData(sUartRing *pRing)
{
  return (pRing != NULL) && !pRing->IsEmpty_R;
}

bool UartRingIsEmpty_R(sUartRing *pRing)
{
  return pRing->IsEmpty_R;
}

bool UartRingIsOver_R(sUartRing *pRing)
{
  return pRing->IsOver_R;
}

void UartRingProcessIRQ_R(sUartRing *pRing, uint8_t val)
{
//TODO assert  if ((pRing == NULL) || (pRing->bufRd._size == 0))
//    return false;

  if (pRing->IsOver_R)          // uz je plno
  {
//TODO maybe stop ??    pRing->_lowLevelDisIRQ_R();          // disable RX interrupt
    return;
  }

  pRing->IsEmpty_R = false;       // kazdopadne nebude prazdny

  pRing->bufRd._buf[pRing->bufRd._wrPtr] = val;

  pRing->bufRd._wrPtr++;
  if (pRing->bufRd._wrPtr >= pRing->bufRd._size)
    pRing->bufRd._wrPtr = 0;

  if (pRing->bufRd._rdPtr == pRing->bufRd._wrPtr)   // dohnal jsem neprectena data ?
  {
    pRing->IsOver_R = true;                       // tak smula pro priste
  }
}

void UartRingDiscard_R(sUartRing *pRing)
{
//  pRing->bufRd._rdPtr = pRing->bufRd._wrPtr = 0;
//  pRing->IsEmpty_R = pRing->IsOver_R = false;

  if(!pRing->IsEmpty_R)
  {
    uint8_t x;
    while(UartRingRecv(pRing, &x))
      ;
  }
}
