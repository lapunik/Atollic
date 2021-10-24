/*
 * oled_1305.h
 *
 *  Created on: 3. 12. 2016
 *      Author: weiss_000
 */

#ifndef OLED_1305_H_
#define OLED_1305_H_

#include "nucleo_core.h"

#ifndef REFRESH_TIMER
#define REFRESH_TIMER   0
#endif

#define OLED_WIDTH_PIX 128
#define OLED_HEIGHT_PIX  32
#define OLED_WIDTH_BYTES 132
#define OLED_HEIGHT_BYTES  4

bool OLED_Init(void);

#endif /* OLED_1305_H_ */
