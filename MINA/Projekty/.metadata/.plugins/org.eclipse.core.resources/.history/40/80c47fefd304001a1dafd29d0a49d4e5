/* Includes */
// je v Nucleo ... #include "stm32f4xx.h"
#include "nucleo_core.h"
#include "nucleo_uart.h"
#include "systick_ms.h"

#include "oled_1305.h"

#include "disp_hilevel.h"

#if (HSE_VALUE != 8000000)
#error HSE_VALUE must be set to 8MHz - parameters of project (or system_stm32f4xx.c) !!
#endif

/* Private macro */
typedef enum {stateTextAtari, stateTextThin, stateText5x7, stateText3x5,
  //NE, ma obracene bity stateText3x6,
  stateBoxes, stateCircles, stateCube, stateLast } eStates;
/* Private variables */
/* Private function prototypes */
/* Private functions */
#include <math.h>

#define COUNT_NODES 8
float node0[] = {-100, -100, -100};
float node1[] = {-100, -100,  100};
float node2[] = {-100,  100, -100};
float node3[] = {-100,  100,  100};
float node4[] = { 100, -100, -100};
float node5[] = { 100, -100,  100};
float node6[] = { 100,  100, -100};
float node7[] = { 100,  100,  100};
float *nodes[COUNT_NODES] = { node0, node1, node2, node3, node4, node5, node6, node7 };

// translate(200, 200); - move center [0,0] to center of area
float center[] = {200, 200};

#define COUNT_EDGES  12
int edge0[] = {0, 1};
int edge1[] = {1, 3};
int edge2[] = {3, 2};
int edge3[] = {2, 0};
int edge4[] = {4, 5};
int edge5[] = {5, 7};
int edge6[] = {7, 6};
int edge7[] = {6, 4};
int edge8[] = {0, 4};
int edge9[] = {1, 5};
int edge10[] = {2, 6};
int edge11[] = {3, 7};
int *edges[COUNT_EDGES]  = {edge0, edge1, edge2, edge3, edge4, edge5, edge6, edge7, edge8, edge9, edge10, edge11};

void rotateZ3D(float theta)
{
  float sin_t = sin(theta * M_PI / 180);
  float cos_t = cos(theta * M_PI / 180);

  for(int i = 0; i < COUNT_NODES; i++)
  {
    float x = nodes[i][0];
    float y = nodes[i][1];
    nodes[i][0] = x * cos_t - y * sin_t;
    nodes[i][1] = y * cos_t + x * sin_t;
  }
}

void rotateY3D (float theta)
{
  float sin_t = sin(theta * M_PI / 180);
  float cos_t = cos(theta * M_PI / 180);

  for(int i = 0; i < COUNT_NODES; i++)
  {
    float x = nodes[i][0];
    float z = nodes[i][2];
    nodes[i][0] = x * cos_t - z * sin_t;
    nodes[i][2] = z * cos_t + x * sin_t;
  }
}

void rotateX3D(float theta)
{
  float sin_t = sin(theta * M_PI / 180);
  float cos_t = cos(theta * M_PI / 180);

  for(int i = 0; i < COUNT_NODES; i++)
  {
    float y = nodes[i][1];
    float z = nodes[i][2];
    nodes[i][1] = y * cos_t - z * sin_t;
    nodes[i][2] = z * cos_t + y * sin_t;
  }
}

void DrawCube(bool color)
{
  for(int i = 0; i < COUNT_EDGES; i++)
  {
    float *n0 = nodes[edges[i][0]];
    float *n1 = nodes[edges[i][1]];

    DISP_DrawLine((n0[0] + center[0]) * 16 / 200, (n0[1] + center[1]) * 16 / 200,
      (n1[0] + center[0]) * 16 / 200, (n1[1] + center[1]) * 16 / 200,
      color);
  }
}

void RedrawCube(float xAng, float yAng, float zAng)
{
  DrawCube(false);

  if (zAng != 0)
    rotateZ3D(zAng);
  if (xAng != 0)
    rotateX3D(xAng);
  if (yAng != 0)
    rotateY3D(yAng);

  DrawCube(true);
}

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)
{
  uint32_t tmBtn = 0, tmAction = 0, intervalAction = 1000;
  bool bbButton, bbLastButton = true;
  eStates state = 0;
  int boxCount = 0;
  bool boxColor = true;
  bool autoRotate = true;

  SystemClock_100MHz();
  SystemCoreClockUpdate();

  InitSystickDefault();

// shared SPI-CLK  Nucleo_SetPinGPIO(GPIOA, 5, output);
//  Nucleo_SetPinGPIO(GPIOC, 13, input_pullup);
  Nucleo_SetPinGPIO(GPIOA, 0, input_pullup);

  Nucleo_StartUart2(38400);

  // pozor, bez tohoto nefunguje putchar a musel by byt fputc(.., stdout)
  setvbuf(stdout,NULL,_IONBF,0);
  setvbuf(stdin,NULL,_IONBF,0);

  puts("\n\nStart APP OLED (ASE) - " __DATE__ " " __TIME__);
  printf("CoreClock: %ld\n", SystemCoreClock);

  OLED_Init();    //! globalne musi byt nastaven REFRESH_TIMER, ktery Timer pro refresh

  WaitMs(50);

  puts("OLED Init finish");

  DISP_Clear();
  DISP_DemoCntRow(2);

  tmBtn = CUR_TICKS - 1;   // aby se to hned spustilo

  DISP_GotoXY(0, 0);

  /* Infinite loop */
  while (1)
  {
    if (CUR_TICKS > tmBtn)
    {
      tmBtn = CUR_TICKS + 10;

//      bbButton = GPIO_READ(GPIOC, 13);
      bbButton = GPIO_READ(GPIOA, 0);

      if (bbLastButton != bbButton)
      {
        bbLastButton = bbButton;

        if (bbButton)       // edge to H
        {
          state++;
          if (state >= stateLast)
            state = 0;

          printf("\nNew State %d\n", state);

          DISP_Clear();

          switch(state)
          {
            case stateTextAtari:
              DISP_SetFont(font_atari_8x8);
              DISP_GotoXY(0, 0);
              DISP_WriteString("ATARI 8x8");
              break;
            case stateTextThin:
              DISP_SetFont(font_thin_8x8);
              DISP_GotoXY(0, 0);
              DISP_WriteString("Thin 8x8");
              break;
            case stateText5x7:
              DISP_SetFont(font_system_5x7);
              DISP_GotoXY(0, 0);
              DISP_WriteString("System 5x7");
              break;
            case stateText3x5:
              DISP_SetFont(font_wendy_3x5);
              DISP_GotoXY(0, 0);
              DISP_WriteString("Wendy 3x5");
              break;
              /*
            case stateText3x6:
              DISP_SetFont(font_newbasic_3x6);
              DISP_GotoXY(0, 0);
              DISP_WriteString("NewBasec 3x6");
              break;
              */
            case stateBoxes:
              boxColor = true;
              boxCount = 0;
              intervalAction = 1000;
              break;
            case stateCircles:
              boxColor = true;
              boxCount = 0;
              intervalAction = 1000;
              break;
            case stateCube:
              rotateY3D(15);
              rotateX3D(5);
              intervalAction = 100;
              break;
            case stateLast:
            default:
              break;
          }
        }
      }
    }

    if (CUR_TICKS > tmAction)
    {
      tmAction = CUR_TICKS + intervalAction;

      putchar('*');

      switch(state)
      {
        case stateTextAtari:
        case stateTextThin:
        case stateText5x7:
        case stateText3x5:
//        case stateText3x6:
          break;
        case stateBoxes:
          DISP_DrawRect(
              boxCount * 5, boxCount,
              OLED_WIDTH_PIX - boxCount * 10 - 1, OLED_HEIGHT_PIX - 2 * boxCount - 1,
              boxColor);
          boxCount += 2;
          if (boxCount >= 10)
          {
            boxCount = 0;
            boxColor = !boxColor;
          }

          break;
        case stateCircles:
          DISP_DrawCircle(
              boxCount * 5 + OLED_HEIGHT_PIX / 2, OLED_HEIGHT_PIX / 2,
              OLED_HEIGHT_PIX / 2 - 1,
              boxColor);
          boxCount += 2;
          if (boxCount >= 10)
          {
            boxCount = 0;
            boxColor = !boxColor;
          }

          break;
        case stateCube:
          if (autoRotate)
          {
            RedrawCube(5, 0, 15);
          }
          break;
        case stateLast:
        default:
          break;
      }
    }

    if (isCharRecv())
    {
      char c = getchar();

      switch (state)
      {
        case stateCube:
          if (c == ' ')
            autoRotate = !autoRotate;

          if (!autoRotate)
          {
            switch(c)
            {
              case 'a':
              case 'A':
                puts("Left");
                RedrawCube(0, 5, 0);
                break;
              case 'd':
              case 'D':
                puts("Right");
                RedrawCube(0, -5, 0);
                break;
              case 's':
              case 'S':
                puts("Down");
                RedrawCube(5, 0, 0);
                break;
              case 'w':
              case 'W':
                puts("Up");
                RedrawCube(-5, 0, 0);
                break;
            }
          }

          switch(c)
          {
            case 'q':
            case 'Q':
              puts("Move Left");
              DrawCube(false);
              center[0] -= 100;
              if (center[0] < 200)
                center[0] = 200;
              DrawCube(true);
              break;
            case 'e':
            case 'E':
              puts("Move Right");
              DrawCube(false);
              center[0] += 100;
              if (center[0] > 1400)
                center[0] = 1400;
              DrawCube(true);
              break;
          }
          break;
        default:
          switch(c)
          {
            default:
              if ((c >= 0) && (c <= 127))
              {
                putchar(c);
                DISP_WriteChar(c);
              }
              else
                putchar('?');
              break;
          }
          break;
      }
    }
  }
}
