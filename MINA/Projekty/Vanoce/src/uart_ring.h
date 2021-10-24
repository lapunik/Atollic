/*
 * uart_ring.h
 *
 *  Created on: 5. 5. 2016
 *      Author: weiss
 */

#ifndef SOURCES_UART_RING_H_
#define SOURCES_UART_RING_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
  uint32_t _wrPtr;
  uint32_t _rdPtr;
  uint8_t *_buf;
  uint32_t _size;
} sUartRingBuffer;

typedef struct
{
  sUartRingBuffer bufWr;
  bool IsEmpty_W;
  bool IsOver_W;
  bool BlockingMode_W;
  sUartRingBuffer bufRd;
  bool IsEmpty_R;
  bool IsOver_R;
  bool BlockingMode_R;
  void (*_lowLevelSend)(uint8_t x);
  void (*_lowLevelEnIRQ_W)(void);
  void (*_lowLevelDisIRQ_W)(void);
  void (*_lowLevelEnIRQ_R)(void);
  void (*_lowLevelDisIRQ_R)(void);
} sUartRing;

sUartRing UartRingCreate(void *bufPtr_W, uint32_t bufSize_W, void *bufPtr_R, uint32_t bufSize_R);

void UartRingInitFn_W(sUartRing *pRing, void *fnEnIRQ_W, void *fnDisIRQ_W, void *fnSend);
bool UartRingSend(sUartRing *pRing, uint8_t b);
void UartRingProcessIRQ_W(sUartRing *pRing);
bool UartRingIsEmpty_W(sUartRing *pRing);
bool UartRingIsOver_W(sUartRing *pRing);
void UartRingDiscard_W(sUartRing *pRing);

void UartRingInitFn_R(sUartRing *pRing, void *fnEnIRQ_R, void *fnDisIRQ_R);
bool UartRingRecv(sUartRing *pRing, uint8_t *pVal);
bool UartRingIsData(sUartRing *pRing);
void UartRingProcessIRQ_R(sUartRing *pRing, uint8_t val);
bool UartRingIsEmpty_R(sUartRing *pRing);
bool UartRingIsOver_R(sUartRing *pRing);
void UartRingDiscard_R(sUartRing *pRing);

#endif /* SOURCES_UART_RING_H_ */
