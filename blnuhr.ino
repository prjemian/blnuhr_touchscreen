// blnuhr
// Copyright 2019, Pete R. Jemian
// license ? CC?

#define TITLE "blnuhr"
#define COPYRIGHT "Copyright 2019, Pete R. Jemians"

// based on:
//   Arduino Mega 2560
//   Velleman VMA412 2.8" 240x320 TouchScreen
//   DS3231 Real Time Clock

#include "blnuhr.h"
#include "blnuhr_rtc.h"
#include "blnuhr_screen.h"
#include "blnuhr_timeset.h"
#include "blnuhr_touch.h"


RtcDS3231<TwoWire> Rtc(Wire);
extern TouchScreen ts;
extern MCUFRIEND_kbv tft;    // LCD panel

uint16_t  display_width, display_height, row_height;
int toggler = 1;
long micros_offset = 0;
bool report_enable = true;

// choose between different operation modes, based on touch screen
//  DISPLAY_CLOCK  |  DISPLAY_TEMPERATURE  |  DISPLAY_TIMESET
enum display_modes_enum  display_mode = DISPLAY_CLOCK;

// for detection of touchpad events
extern enum ts_touch_states_enum   touch_state;
extern enum ts_touch_states_enum   touch_state_previous;
extern unsigned long  time_pressed;
extern unsigned long  touch_duration;
enum display_modes_enum   next_display_mode;

unsigned long  show_temperature_end_time, show_timeset_end_time;


void setup() {
  unsigned long  time_0 = millis();
  unsigned long  time_00 = time_0;

  Serial.begin(COMM_SPEED);
  for (int i = 10; !Serial && i; i--)  // Serial might be slow to start
    delay(500);
  Serial.println("blnuhr-touch");
  Serial.println("(c) 2019, Pete R. Jemian");
  Serial.println();

  uint16_t ID = tft.readID();
  if (ID == 0xD3D3) ID = 0x9481; // write-only shield
  tft.begin(ID);
  tft.setRotation(DISPLAY_ROTATION);
  display_width = tft.width() - BORDER*2;
  display_height = tft.height() - BORDER*2;
  row_height = (display_height - BORDER*4)/5;
  drawStandardBackground();
  
  Rtc.Begin();
  printLabeledTime("RTC: ", Rtc.GetDateTime());
  setup_RTC(Rtc);

  if (display_mode == DISPLAY_TEMPERATURE) {
    show_temperature_end_time = millis() + SHOW_TEMPERATURE_DURATION_MS;
    start_temperature();
  } else if (display_mode == DISPLAY_TIMESET) {
    RtcDateTime now = Rtc.GetDateTime();
    show_timeset_end_time = millis() + SHOW_TIMESET_DURATION_MS;
    start_timeset(now);
  }

  Serial.println("setup() duration: " + String(0.001*(millis() - time_00), 3) + "s");

  tft.setTextSize(2);
  tft.setCursor(BORDER*2, BORDER*2);
  tft.print(TITLE);
  tft.setTextSize(1);
  tft.setCursor(BORDER*2, BORDER*5);
  tft.print(COPYRIGHT);
  tft.setCursor(BORDER*2, BORDER*10);
  tft.print("fur Dieter Fuelleborn");
  delay(5000);
  drawStandardBackground();
}


void loop() {
  TSPoint p = getTouchPoint();
  unsigned long   event_time_stamp = millis(), press_duration;
  int btn_index;
  int pixel_x, pixel_y;

  ts2pixel(p, &pixel_x, &pixel_y);

  if (!Rtc.IsDateTimeValid()) {
      // Common Causes:
      //    #1 the RTC battery on the device is low
      //    #2 the RTC battery on the device is missing
      //    #3 the power line was disconnected
      Serial.println("RTC lost confidence in the DateTime!");
  }
  RtcDateTime now = Rtc.GetDateTime();

  // print to the console occasionally
  if (now.Second() % REPORTING_INTERVAL_S == 0) {
    if (report_enable) {
      printDateTime(now);
      RtcTemperature temp = Rtc.GetTemperature();
      Serial.print(" " + String(temp.AsFloatDegC()) + " C");
      if ((now.Month() == 9) && (now.Day() == 24)) {
        Serial.print(" -- Happy Birthday, Dieter!");
      }
      Serial.println();
      if (micros_offset == 0) {
        micros_offset = micros();
      }
      report_enable = false;
    }
  } else {
    report_enable = true;
  }

  if (display_mode == DISPLAY_CLOCK) {
    setLights(now);
    check_touch_in_clock_mode();

  } else if (display_mode == DISPLAY_TEMPERATURE) {
    loop_temperature();
    // switch back to show clock after a short interval
    bool expired = (millis() >= show_temperature_end_time) ? true : false;
    if ((touch_state == TS_RELEASED) && (touch_duration > 100)) {
      expired = true;
    }
    if (expired) {
      display_mode = DISPLAY_CLOCK;
      drawStandardBackground();
    }

  } else if (display_mode == DISPLAY_TIMESET) {
    bool stay_in_timeset = loop_timeset(p, pixel_x, pixel_y);
    bool expired = (millis() >= show_timeset_end_time) ? true : false;
    if (!stay_in_timeset || expired) {
      //    Serial.println("loop(): stay_in_timeset=" + String(stay_in_timeset) \
      //      + " time expired=" + String(expired) \
      //      + " millis()=" + String(millis()) \
      //      + " show_timeset_end_time=" + String(show_timeset_end_time));
      display_mode = DISPLAY_CLOCK;
      drawStandardBackground();
    }
  }

  delay(LOOP_DELAY);
  toggler = toggler ? 0 : 1;
}


void check_touch_in_clock_mode() {
//  Serial.println("Display mode: " + String(display_mode));
  int x, y;
  x = tft.width() - BORDER - 40;
  y = BORDER+2;
  if (touch_state == TS_PRESSED) {
    // this rectangle gives user some feedback during a screen press
    tft.fillRect(x, y, 38, 18, TFT_CYAN);
    next_display_mode = DISPLAY_TEMPERATURE;

  } else if ((touch_state == TS_DOWN) || (touch_state == TS_FLICKER)) {
    unsigned int duration = millis() - time_pressed;
    if ((TRIGGER_TIMESET_PRESS_DURATION_MS <= duration) && (duration < TIMEOUT_TIMESET_PRESS_DURATION_MS)) {
      tft.fillRect(x, y, 38, 18, TFT_GREEN);
      next_display_mode = DISPLAY_TIMESET;
    } else if (duration > TIMEOUT_TIMESET_PRESS_DURATION_MS) {
      tft.fillRect(x, y, 38, 18, TFT_RED);
      next_display_mode = DISPLAY_CLOCK;
    }

  } else if (touch_state == TS_RELEASED) {
    tft.fillRect(x, y, 38, 18, BACKGROUND);
    display_mode = next_display_mode;
    if (display_mode == DISPLAY_TEMPERATURE) {
        show_temperature_end_time = millis() + SHOW_TEMPERATURE_DURATION_MS;
        start_temperature();
        display_mode = DISPLAY_TEMPERATURE;
    } else if (display_mode == DISPLAY_TIMESET) {
        //  Serial.println("TIMESET mode selected:  ");
        Serial.println("from millis()=" + String(millis()));
        Serial.println("SHOW_TIMESET_DURATION_MS=" + String(SHOW_TIMESET_DURATION_MS));
        show_timeset_end_time = millis() + SHOW_TIMESET_DURATION_MS;
        Serial.println("Setting show_timeset_end_time=" + String(show_timeset_end_time));
        RtcDateTime now = Rtc.GetDateTime();
        start_timeset(now);
        touch_state = TS_IDLE;
    }
  }
}

