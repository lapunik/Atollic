/*
 * disp_hilevel.c
 *
 *  Created on: 3. 12. 2016
 *      Author: weiss_000
 */

#include "disp_hilevel.h"

static void (* my_DrawPixel)(int x, int y, bool barva) = NULL;
static void (* my_SetByte)(int x, int row, uint8_t val) = NULL;
static void (* my_SetBytes)(int x, int row, uint8_t *data, int len) = NULL;

static int pixelsWidth, pixelsHeight;
static int textX = 0, textY = 0;

void SetHiLevelDisp(int pixWidth, int pixHeight,
    void *ptrDrawPixel, void *ptrSetByte, void *ptrSetBytes)
{
  my_DrawPixel = ptrDrawPixel;
  my_SetByte = ptrSetByte;
  my_SetBytes = ptrSetBytes;

  pixelsWidth = pixWidth;
  pixelsHeight = pixHeight;
}

#include "font_atari_8x8.h"
#include "font_thin_8x8.h"
#include "font_system_5x7.h"
#include "font_wendy_3x5.h"
#include "font_newbasic_3x6.h"

static uint8_t *bpFontBase = NULL;
static FONT_Header_t *ptrFontHeader = NULL;

bool DISP_SetFont(eFontsAvailable fnt)
{
  switch(fnt)
  {
    case font_atari_8x8:
    default:
      ptrFontHeader = (FONT_Header_t *)font_atari_8x8_data;

      //TODO select start of char defines, fefaultne ZA hlavickou
      bpFontBase = font_atari_8x8_data + sizeof(FONT_Header_t);
      break;
    case font_thin_8x8:
      ptrFontHeader = (FONT_Header_t *)font_thin_8x8_data;
      bpFontBase = font_thin_8x8_data + sizeof(FONT_Header_t);
      break;
    case font_system_5x7:
      ptrFontHeader = (FONT_Header_t *)font_system_5x7_data;
      bpFontBase = font_system_5x7_data + sizeof(FONT_Header_t);
      break;
    case font_wendy_3x5:
      ptrFontHeader = (FONT_Header_t *)font_wendy_3x5_data;
      bpFontBase = font_wendy_3x5_data + sizeof(FONT_Header_t);
      break;
    case font_newbasic_3x6:
      ptrFontHeader = (FONT_Header_t *)font_newbasic_3x6_data;
      bpFontBase = font_newbasic_3x6_data + sizeof(FONT_Header_t);
      break;
  }

  return true;
}

int DISP_GetCharPerRow(void)
{
  return pixelsWidth / ptrFontHeader->width;
}

int DISP_GetRowPerDisp(void)
{
  return pixelsHeight / ptrFontHeader->height;
}

void DISP_GotoXY(int x, int y)
{
  textX = x;
  textY = y;
}

void DISP_WriteChar(char c)
{
  DISP_WriteCharXY(textX, textY, c);

  textX++;
  if (textX >= pixelsWidth / ptrFontHeader->width)
  {
    textX = 0;

    textY++;
    if (textY >= pixelsHeight / ptrFontHeader->height)
      textY = 0;
  }
}

void DISP_WriteString(char *cp)
{
  while(*cp)
  {
    DISP_WriteChar(*cp);
    cp++;
  }
}

void DISP_WriteCharXY(int x, int y, char c)
{
  int i, j;
  uint8_t *bp;

  if (ptrFontHeader == NULL)
    DISP_SetFont(-1);     // use default font

  if ((c >= (ptrFontHeader->first + ptrFontHeader->count)) || (c < ptrFontHeader->first))
    return;

  c -= ptrFontHeader->first;
  bp = bpFontBase + (c * ptrFontHeader->width);

  if (ptrFontHeader->height == 8)     // pro vysku 8px vyhodne po bajtech
  {
    if (my_SetBytes != NULL)
    {
      my_SetBytes(x * 8, y, bp, 8);
      return;
    }

    if (my_SetByte != NULL)
    {
      for (i = 0; i < 8; i++)
        my_SetByte(x * 8 + i, y, bp[i]);

      return;
    }
  }

  for (i = 0; i < ptrFontHeader->width; i++)
  {
    uint8_t b = bp[i];

    if (ptrFontHeader->height <= 8)
    {
      for (j = 0; j < ptrFontHeader->height; j++)
      {
        my_DrawPixel(x * ptrFontHeader->width + i, y * ptrFontHeader->height + j, (b & 1));    // LSB first
        b >>= 1;
      }

      continue;
    }

    /* NESMI
    if (ptrFontHeader->height <= 16)
    {
      for (j = 0; j < 8; j++)
      {
        my_DrawPixel(x * 8 + i, y + j, (b & 1));    // LSB first
        b >>= 1;
      }

      continue;
    }
    */
  }
}

bool DISP_Clear()
{
  return DISP_Fill(0x00);
}

bool DISP_Fill(uint8_t bFill)
{
  int r, x;

  if (my_SetByte != NULL)
  {
    for (r = 0; r < pixelsHeight / 8; r++)
    {
      for (x = 0; x < pixelsWidth; x++)
        my_SetByte(x, r, bFill);
    }
  }
  else
    return false;

  return true;
}

bool DISP_DemoCntRow(uint8_t bRow)
{
  int x;

  if (my_SetByte != NULL)
  {
    for (x = 0; x < pixelsWidth; x++)
      my_SetByte(x, bRow, x);
  }
  else
    return false;

  return true;
}

void DISP_DrawLine(int x0, int y0, int x1, int y1, bool color)
{
  int dx = (x0 < x1) ? (x1 - x0) : (x0 - x1), sx = (x0 < x1) ? 1 : -1;
  int dy = (y0 < y1) ? (y1 - y0) : (y0 - y1), sy = (y0 < y1) ? 1 : -1;
  int err = ((dx > dy) ? dx : -dy) / 2, e2;

  for (; ; )
  {
    if ((x0 == x1) && (y0 == y1))
      break;

    (*my_DrawPixel)((uint8_t)x0, (uint8_t)y0, color);

    e2 = err;
    if (e2 > -dx)
    {
      err -= dy; x0 += sx;
    }

    if (e2 < dy)
    {
      err += dx; y0 += sy;
    }
  }
}

void DISP_DrawRect(int x, int y, int w, int h, bool color)
{
  DISP_DrawLine(x, y, x + w, y, color);
  DISP_DrawLine(x + w, y, x + w, y + h, color);
  DISP_DrawLine(x + w, y + h, x, y + h, color);
  DISP_DrawLine(x, y + h, x, y, color);
}

void DISP_FillRect(int x, int y, int w, int h, bool color)
{
  int ww = w, xx = x;

  for(; h; h--)
  {
    x = xx;

    for(w = ww; w; w--)
    {
      (*my_DrawPixel)(x, y, color);
      x++;
    }

    y++;
  }
}

void DISP_DrawCircle(int centerX, int centerY, int radius, bool colorSet)
{
  int d = (5 - radius * 4) / 4;
  int x = 0;
  int y = radius;

  do
  {
    // ensure index is in range before setting (depends on your image implementation)
    // in this case we check if the pixel location is within the bounds of the image before setting the pixel
    if (((centerX + x) >= 0) && ((centerX + x) <= (pixelsWidth - 1)) && ((centerY + y) >= 0) && ((centerY + y) <= (pixelsHeight - 1))) (*my_DrawPixel)(centerX + x, centerY + y, colorSet);
    if (((centerX + x) >= 0) && ((centerX + x) <= (pixelsWidth - 1)) && ((centerY - y) >= 0) && ((centerY - y) <= (pixelsHeight - 1))) (*my_DrawPixel)(centerX + x, centerY - y, colorSet);
    if (((centerX - x) >= 0) && ((centerX - x) <= (pixelsWidth - 1)) && ((centerY + y) >= 0) && ((centerY + y) <= (pixelsHeight - 1))) (*my_DrawPixel)(centerX - x, centerY + y, colorSet);
    if (((centerX - x) >= 0) && ((centerX - x) <= (pixelsWidth - 1)) && ((centerY - y) >= 0) && ((centerY - y) <= (pixelsHeight - 1))) (*my_DrawPixel)(centerX - x, centerY - y, colorSet);
    if (((centerX + y) >= 0) && ((centerX + y) <= (pixelsWidth - 1)) && ((centerY + x) >= 0) && ((centerY + x) <= (pixelsHeight - 1))) (*my_DrawPixel)(centerX + y, centerY + x, colorSet);
    if (((centerX + y) >= 0) && ((centerX + y) <= (pixelsWidth - 1)) && ((centerY - x) >= 0) && ((centerY - x) <= (pixelsHeight - 1))) (*my_DrawPixel)(centerX + y, centerY - x, colorSet);
    if (((centerX - y) >= 0) && ((centerX - y) <= (pixelsWidth - 1)) && ((centerY + x) >= 0) && ((centerY + x) <= (pixelsHeight - 1))) (*my_DrawPixel)(centerX - y, centerY + x, colorSet);
    if (((centerX - y) >= 0) && ((centerX - y) <= (pixelsWidth - 1)) && ((centerY - x) >= 0) && ((centerY - x) <= (pixelsHeight - 1))) (*my_DrawPixel)(centerX - y, centerY - x, colorSet);
    if (d < 0)
    {
      d += 2 * x + 1;
    }
    else
    {
      d += 2 * (x - y) + 1;
      y--;
    }
    x++;
  } while (x <= y);
}

void DISP_FillCircle(int x0, int y0, int radius, bool color)
{
  int f = 1 - radius;
  int ddF_x = 0;
  int ddF_y = -2 * (int)radius;
  int x = 0;
  int y = (int)radius;
  int t1, t2;

  (*my_DrawPixel)(x0, y0 + radius, color);
  t1 = y0 - radius;
  (*my_DrawPixel)(x0, (t1 > 0) ? t1 : 0, color);
  t1 = x0 - radius;
  DISP_DrawLine(x0 + radius, y0, (t1 > 0) ? t1 : 0, y0, color);

  while(x < y)
  {
    if(f >= 0)
    {
        y--;
        ddF_y += 2;
        f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x + 1;

    t1 = x0 - x;
    t2 = y0 - y;
    DISP_DrawLine((t1 > 0) ? t1 : 0, y0 + y, x0 + x, y0 + y, color);
    DISP_DrawLine((t1 > 0) ? t1 : 0, (t2 > 0) ? t2 : 0, x0 + x, (t2 > 0) ? t2 : 0, color);
    t1 = x0 - y;
    t2 = y0 - x;
    DISP_DrawLine((t1 > 0) ? t1 : 0, y0 + x, x0 + y, y0 + x, color);
    DISP_DrawLine((t1 > 0) ? t1 : 0, (t2 > 0) ? t2 : 0, x0 + y, (t2 > 0) ? t2 : 0, color);
  }
}
