// blnuhr_timeset.ino

#include "blnuhr_timeset.h"
#include "blnuhr_screen.h"
#include "blnuhr_touch.h"

Adafruit_GFX_Button btn_ok, btn_cancel;
Adafruit_GFX_Button btn_h12_up, btn_h12_dn;
Adafruit_GFX_Button btn_h1_up, btn_h1_dn;
Adafruit_GFX_Button btn_m10_up, btn_m10_dn;
Adafruit_GFX_Button btn_m1_up, btn_m1_dn;

Adafruit_GFX_Button *buttons[] = { \
  &btn_cancel, &btn_ok, \
  &btn_h12_up, &btn_h12_dn, \
  &btn_h1_up, &btn_h1_dn, \
  &btn_m10_up, &btn_m10_dn, \
  &btn_m1_up, &btn_m1_dn, \
  NULL};

extern bool touch_pressed;
extern enum ts_touch_states_enum   touch_state;
int button_index;
RtcDateTime edited_time;


void start_timeset(RtcDateTime the_time) {
  int bx, by, bw, bh;
  int font_size = 2;
  tft.fillRect(BORDER, BORDER, tft.width()-2*BORDER, tft.height()-2*BORDER, TFT_DARKGREY);
  tft.setFont();
  
  bw = 90;    // button width
  bh = 40;    // button height

  bx = BORDER+20;
  by = tft.height() - BORDER -10 - bh;
  btn_ok.initButtonUL(&tft, bx, by, bw, bh, \
    TFT_WHITE, TFT_LIGHTGREY, TFT_BLACK, "OK", font_size);
  btn_ok.drawButton(false);
  
  bx = tft.width() - BORDER -20 - bw;
  btn_cancel.initButtonUL(&tft, bx, by, bw, bh, \
    TFT_WHITE, TFT_LIGHTGREY, TFT_BLACK, "Cancel", font_size);
  btn_cancel.drawButton(false);

  bw = (tft.width() - 5*BORDER)/4;    // 4 buttons across
  bx = BORDER;
  by = BORDER;
  bh = 30;
  btn_h12_up.initButtonUL(&tft, bx, by, bw, bh, \
    TFT_WHITE, TFT_LIGHTGREY, TFT_BLACK, "+12h", 1);
  btn_h12_up.drawButton(false);
  bx += bw + BORDER;
  btn_h1_up.initButtonUL(&tft, bx, by, bw, bh, \
    TFT_WHITE, TFT_LIGHTGREY, TFT_BLACK, "+1h", 1);
  btn_h1_up.drawButton(false);
  bx += bw + BORDER;
  btn_m10_up.initButtonUL(&tft, bx, by, bw, bh, \
    TFT_WHITE, TFT_LIGHTGREY, TFT_BLACK, "+10m", 1);
  btn_m10_up.drawButton(false);
  bx += bw + BORDER;
  btn_m1_up.initButtonUL(&tft, bx, by, bw, bh, \
    TFT_WHITE, TFT_LIGHTGREY, TFT_BLACK, "+1m", 1);
  btn_m1_up.drawButton(false);

  bw = (tft.width() - 5*BORDER)/4;    // 4 buttons across
  bx = BORDER;
  by = tft.height() - BORDER-90;
  bh = 30;
  btn_h12_dn.initButtonUL(&tft, bx, by, bw, bh, \
    TFT_WHITE, TFT_LIGHTGREY, TFT_BLACK, "-12h", 1);
  btn_h12_dn.drawButton(false);
  bx += bw + BORDER;
  btn_h1_dn.initButtonUL(&tft, bx, by, bw, bh, \
    TFT_WHITE, TFT_LIGHTGREY, TFT_BLACK, "-1h", 1);
  btn_h1_dn.drawButton(false);
  bx += bw + BORDER;
  btn_m10_dn.initButtonUL(&tft, bx, by, bw, bh, \
    TFT_WHITE, TFT_LIGHTGREY, TFT_BLACK, "-10m", 1);
  btn_m10_dn.drawButton(false);
  bx += bw + BORDER;
  btn_m1_dn.initButtonUL(&tft, bx, by, bw, bh, \
    TFT_WHITE, TFT_LIGHTGREY, TFT_BLACK, "-1m", 1);
  btn_m1_dn.drawButton(false);

  edited_time = the_time;
  drawDateTime();
}


bool loop_timeset(TSPoint p, int x, int y) {
  bool keep_looping = true;
  if (touch_state == TS_PRESSED) {
    button_index = getButtonIndex(x, y, buttons);
  } else 
  if ((touch_state == TS_RELEASED) && (button_index != NO_BUTTON)) {
    keep_looping = process_button_press(button_index);
  }
  return(keep_looping);
}


int getButtonIndex(int x, int y, Adafruit_GFX_Button **pb) {
  int btn = NO_BUTTON;
  for (int i = 0; buttons[i] != NULL; i++)
    if (pb[i]->contains(x, y))  btn = i;
  return (btn);
}


void drawDigit(int x, int y, int digit) {
  tft.setFont(&FreeSevenSegNumFont);
  tft.setTextColor(TFT_WHITE, TFT_LIGHTGREY);
  tft.setTextSize(0);
  tft.setCursor(x, y);
  tft.print(String(digit));
}


void eraseDigit(int x, int y, int digit) {
  tft.setFont(&FreeSevenSegNumFont);
  tft.setTextColor(TFT_DARKGREY, TFT_DARKGREY);
  tft.setTextSize(0);
  tft.setCursor(x, y);
  tft.print(String(digit));
}


void drawDateTime() {
    char hms[12], ymd[12];
    int x, y, w, h;
    
    y = 150;

    snprintf_P(hms, 
            countof(hms),
            PSTR("%0u:%02u:%02u"),
            edited_time.Hour(),
            edited_time.Minute(),
            edited_time.Second() );
    tft.setFont(&FreeSansBold12pt7b);
    tft.setTextSize(2);
    tft.getTextBounds(hms, 0, y, &x, &y, &w, &h);
    // Serial.println("x="+String(x)+" y="+String(y)+" w="+String(w)+" h="+String(h));
    x = tft.width()/2 - w/2;
    tft.fillRect(x-20, y-h, w+44, h+4, TFT_DARKGREY);
    tft.setCursor(x, y);
    tft.print(hms);

    snprintf_P(ymd, 
            countof(ymd),
            PSTR("%04u-%02u-%02u"),
            edited_time.Year(),
            edited_time.Month(),
            edited_time.Day() );

    y = 88;
    tft.setFont(&FreeSansBold12pt7b);
    tft.setTextSize(1);
    tft.getTextBounds(ymd, 0, y, &x, &y, &w, &h);
    // Serial.println("x="+String(x)+" y="+String(y)+" w="+String(w)+" h="+String(h));
    x = tft.width()/2 - w/2;
    tft.fillRect(x, y-h, w+4, h+4, TFT_DARKGREY);
    tft.setCursor(x, y);
    tft.print(ymd);

    printLabeledTime("edited_time: ", edited_time);
}

bool process_button_press(int button_index) {
  int digit, modulo;
  bool keep_looping = true;
  if (button_index == BTN_CANCEL) {
    //  Serial.println("Not changing RTC");
    keep_looping = false;

  } else if (button_index == BTN_OK) {
    printLabeledTime("Updating RTC: ", edited_time);
    Rtc.SetDateTime(edited_time);
    keep_looping = false;

  } else if (button_index == BTN_H12_UP) {
    //  Serial.println("increment 10h button");
    edited_time += HOUR_TWELVE;            // increment by 12h
    drawDateTime();
  
  } else if (button_index == BTN_H12_DN) {
    //  Serial.println("decrement 10h button");
    edited_time -= HOUR_TWELVE;            // decrement by 12h
    drawDateTime();
  
  } else if (button_index == BTN_H1_UP) {
    //  Serial.println("increment 1h button");
    edited_time += HOUR_ONE;               // increment by 1h
    drawDateTime();
  
  } else if (button_index == BTN_H1_DN) {
    //  Serial.println("decrement 1h button");
    edited_time -= HOUR_ONE;               // decrement by 1h
    drawDateTime();
  
  } else if (button_index == BTN_M10_UP) {
    //  Serial.println("increment 10m button");
    edited_time += MINUTE_TEN;               // increment by 10m
    drawDateTime();
  
  } else if (button_index == BTN_M10_DN) {
    //  Serial.println("decrement 10m button");
    edited_time -= MINUTE_TEN;               // decrement by 10m
    drawDateTime();
  
  } else if (button_index == BTN_M1_UP) {
    //  Serial.println("increment 1m button");
    edited_time += MINUTE_ONE;                  // increment by 1m
    drawDateTime();
  
  } else if (button_index == BTN_M1_DN) {
    //  Serial.println("decrement 1m button");
    edited_time -= MINUTE_ONE;                  // decrement by 1m
    drawDateTime();
  }

  return(keep_looping);
}
