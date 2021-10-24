/*
 * disp_hilevel.h
 *
 *  Created on: 3. 12. 2016
 *      Author: weiss_000
 */

#ifndef DISP_HILEVEL_H_
#define DISP_HILEVEL_H_

#include <stdint.h>
#include <stdbool.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

void SetHiLevelDisp(int pixWidth, int pixHeight,
    void *ptrDrawPixel, void *ptrSetByte, void *ptrSetBytes);

void DISP_DrawLine(int x0, int y0, int x1, int y1, bool color);
void DISP_DrawRect(int x, int y, int w, int h, bool color);
void DISP_FillRect(int x, int y, int w, int h, bool color);
void DISP_DrawCircle(int centerX, int centerY, int radius, bool colorSet);
void DISP_FillCircle(int x0, int y0, int radius, bool color);

void DISP_WriteCharXY(int x, int y, char c);
void DISP_WriteChar(char c);
void DISP_WriteString(char *cp);
void DISP_GotoXY(int x, int y);
bool DISP_Clear(void);
bool DISP_Fill(uint8_t bFill);
bool DISP_DemoCntRow(uint8_t bRow);

typedef enum {font_atari_8x8, font_thin_8x8, font_system_5x7,
  font_wendy_3x5, font_newbasic_3x6 } eFontsAvailable;
bool DISP_SetFont(eFontsAvailable fnt);
int DISP_GetCharPerRow(void);
int DISP_GetRowPerDisp(void);

#endif /* DISP_HILEVEL_H_ */
