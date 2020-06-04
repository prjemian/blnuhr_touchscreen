// blnuhr_screen.h

// uses: Adafruit GFX Library 1.8.4
// uses: Adafruit TFTLCD Library 1.0.2
// uses: MCUFRIEND_kbv 2.9.8

#ifndef BLNUHR_SCREEN
#define BLNUHR_SCREEN

#include "Adafruit_GFX.h"       // Hardware-specific library
#include <Adafruit_TFTLCD.h>    // touch-screen library
#include <FreeDefaultFonts.h>
#include <Fonts/FreeSansBoldOblique24pt7b.h>
#include <MCUFRIEND_kbv.h>
#include <Wire.h>
#include "blnuhr_rtc.h"
#include "blnuhr_touch.h"


// screen has two parts:
//   LCD
//   touch panel (overlays the LCD)

#define PORTRAIT  0
#define LANDSCAPE 1

#define DISPLAY_ROTATION  LANDSCAPE
//#define DISPLAY_ROTATION  PORTRAIT
#define BORDER  (DISPLAY_ROTATION == PORTRAIT ? 8 : 12)

// VMA412 LCD connections to Arduino Mega 2560
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// Assign colors we'll use:
#define RED_OFF     0x8148
#define ORANGE_OFF  0x82A8
#define BACKGROUND  0x18E3

void setLights(RtcDateTime now);
void drawSeconds(int visible);
void drawHour(int hr);
void drawMinute(int m);
void drawH15(int hr);
void drawH1(int hr);
void drawM15(int m);
void drawM1(int m);
void drawStandardBackground();

void start_temperature();
void loop_temperature();

#endif // BLNUHR_SCREEN
