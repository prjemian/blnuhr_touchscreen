// blnuhr_screen.ino

#include "blnuhr_screen.h"
#include "blnuhr_timeset.h"

MCUFRIEND_kbv tft;
enum button_names_enum button_pressed = NO_BUTTON;
enum button_names_enum button_selected = NO_BUTTON;


void setLights(RtcDateTime now) {
  int strobe = 0;
  long mu;
  int pwm;
  int h, m;
      
  // strobe the seconds LED on/off, 2s cycle
  mu = micros() - micros_offset;
  strobe = long(mu/1000000.) % 2;

  drawSeconds(strobe);

  // this is the blnuhr display protocol
  drawHour(now.Hour());
  drawMinute(now.Minute());
}


void drawSeconds(int visible) {
  uint16_t h, v, radius, color;
  radius = row_height/2;
  v = BORDER + radius;
  h = tft.width()/2;
  color = (visible) ? TFT_ORANGE : ORANGE_OFF;
  tft.fillCircle(h, v, radius, color);
}


void drawHour(int hr) {
  drawH15(hr);
  drawH1(hr);
}


void drawMinute(int m) {
  drawM15(m);
  drawM1(m);
}


void drawH15(int hr) {
  uint16_t top, left, h, v, radius, color;
  hr = (hr == 0) ? 24 : hr;     // show all lights on, for nightowls
  radius = BORDER/4;
  v = row_height;
  top = BORDER + 1*(v + BORDER);
  left = BORDER;
  h = (display_width - BORDER*3)/4;
  for (int t = 5; t<25; t+=5) {
    color = (hr >= t) ? TFT_RED : RED_OFF;
    tft.fillRoundRect(left, top, h, v, radius, color);
    left += BORDER + h;
  }
}


void drawH1(int hr) {
  uint16_t top, left, h, v, radius, color;
  hr = (hr == 0) ? 24 : hr;     // show all lights on, for nightowls
  radius = BORDER/4;
  v = row_height;
  top = BORDER + 2*(v + BORDER);
  left = BORDER;
  h = (display_width - BORDER*3)/4;
  for (int t = 0; t<5; t++) {
    color = ((hr % 5) > t) ? TFT_RED : RED_OFF;
    tft.fillRoundRect(left, top, h, v, radius, color);
    left += BORDER + h;
  }
}


void drawM15(int m) {
  uint16_t top, left, h, v, radius, color, color_on, color_off;
  radius = BORDER/4;
  v = row_height;
  top = BORDER + 3*(v + BORDER);
  left = BORDER;
  h = (display_width - BORDER*10)/11;
  for (int t = 5; t<59; t+=5) {
    color_on  = ((t%15) == 0) ? TFT_RED : TFT_ORANGE;
    color_off = ((t%15) == 0) ? RED_OFF : ORANGE_OFF;
    color = (m >= t) ? color_on : color_off;
    tft.fillRoundRect(left, top, h, v, radius, color);
    left += BORDER + h;
  }
}


void drawM1(int m) {
  uint16_t top, left, h, v, radius, color;
  radius = BORDER/4;
  v = row_height;
  top = BORDER + 4*(v + BORDER);
  left = BORDER;
  h = (display_width - BORDER*3)/4;
  for (int t = 0; t<5; t++) {
    color = ((m % 5) > t) ? TFT_ORANGE : ORANGE_OFF;
    tft.fillRoundRect(left, top, h, v, radius, color);
    left += BORDER + h;
  }
}


void drawStandardBackground() {
  tft.fillRect(0, 0, tft.width(), tft.height(), TFT_BLACK);
  tft.fillRect(BORDER/2, BORDER/2, display_width+BORDER, display_height+BORDER, BACKGROUND);
}

bool _show_temperature_ = false;

/*
 * prepare to show current temperature on screen
 */
void start_temperature() {
  _show_temperature_ = false;
  drawStandardBackground();
}

float last_temperature;
float this_temperature;

/*
 * show current temperature on screen
 */
void loop_temperature() {
  RtcTemperature temp = Rtc.GetTemperature();

  int x, y;
  x = tft.width()/2 - 100;
  y = tft.height()/2 + 24;
  
  this_temperature = temp.AsFloatDegC();
  
  // first time is always a few pixels low, skip it
  tft.setFont(&FreeSansBoldOblique24pt7b);
  tft.setTextSize(1);
  if (_show_temperature_) {
    if (this_temperature != last_temperature) {
      // erase the old value
      tft.setCursor(x, y);
      tft.setTextColor(BACKGROUND, BACKGROUND);
      tft.print(String(last_temperature) + " C");
    }
    tft.setCursor(x, y);
    tft.setTextColor(TFT_WHITE, BACKGROUND);
    tft.print(String(this_temperature) + " C");
  } else {
    tft.print("");
    _show_temperature_ = true;
  }
  last_temperature = this_temperature;
}

