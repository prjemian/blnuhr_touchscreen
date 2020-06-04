// blnuhr_touch.h

#include "blnuhr_touch.h"
#include "blnuhr_screen.h"


float touch_pressure = 0;
bool touch_pressed = false;
unsigned long  ts_flicker_start;
enum ts_touch_states_enum   touch_state = TS_IDLE;
enum ts_touch_states_enum   touch_state_previous = touch_state;
unsigned long  time_pressed = (unsigned long) NULL;
unsigned long  touch_duration = (unsigned long) NULL;

//  #define RX_CONSTANT 300
#define RX_CONSTANT 0
TouchScreen ts = TouchScreen(XP, YP, XM, YM, RX_CONSTANT);

/*
   getTouchPoint()

   return a TSPoint object with the current TouchScreen event

   also compute smooth ``touch_pressure`` value that debounces the unstable raw value
*/
TSPoint getTouchPoint() {
  TSPoint p = ts.getPoint();

  // restore pins shared with TFT
  pinMode(YP, OUTPUT);
  pinMode(XM, OUTPUT);
  digitalWrite(YP, HIGH);
  digitalWrite(XM, HIGH);

  // global variables: touch_pressure, touch_smooth, touch_state, touch_state_previous

  // debouncing
  touch_pressure = max(0, SMOOTH(touch_pressure, p.z));
  // Serial.println(String(p.x) + ", " + String(p.y) + ", " + String(p.z) + ", " + String(touch_pressure));

  touch_pressed = (TS_MINPRESSURE <= touch_pressure) && (touch_pressure < TS_MAXPRESSURE);
  bool p_pressed = (TS_MINPRESSURE <= p.z) && (p.z < TS_MAXPRESSURE);
  bool flicker = (p_pressed != touch_pressed);  // this is why we need to smooth!

  // determine the state: TS_IDLE, TS_PRESSED, TS_DOWN, TS_FLICKERING, TS_RELEASED
  touch_state_previous = touch_state;
  if (touch_state_previous == TS_IDLE) {
    touch_state = touch_pressed ? TS_PRESSED : TS_IDLE;

  } else if (touch_state_previous == TS_PRESSED) {
    touch_state = touch_pressed ? TS_DOWN : TS_FLICKER;
    if (touch_state == TS_FLICKER) ts_flicker_start = millis();

  } else if (touch_state_previous == TS_DOWN) {
    touch_state = touch_pressed ? TS_DOWN : TS_FLICKER;
    if (touch_state == TS_FLICKER) ts_flicker_start = millis();

  } else if (touch_state_previous == TS_FLICKER) {
    if (touch_pressed)
      touch_state = TS_DOWN;
    else if ((millis() - ts_flicker_start) > TS_FLICKER_DELAY_MS)
      touch_state = (touch_pressed) ? TS_DOWN : TS_RELEASED;

  } else if (touch_state_previous == TS_RELEASED) {
    touch_state = touch_pressed ? /* long flicker */ TS_PRESSED : TS_IDLE;
    //if (touch_state == TS_PRESSED)
    //  Serial.println("\n\n unexpected transition: TS_RELEASED -> TS_PRESSED");

    // CAUTION:  caller should consider TS_RELEASED -> TS_PRESSED transitions
  }

  if (touch_state == TS_PRESSED)
    time_pressed = millis();
  else if (touch_state == TS_PRESSED)
    touch_duration = millis() - time_pressed;

  return (p);
}


void ts2pixel(TSPoint p, int *x, int *y) {
  if (DISPLAY_ROTATION == PORTRAIT) {
    *x = map(p.x, TS_LEFT, TS_RT, 0, tft.width());
    *y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
  } else {    // LANDSCAPE
    *x = map(p.y, TS_TOP, TS_BOT, 0, tft.width());
    *y = map(p.x, TS_RT, TS_LEFT, 0, tft.height());
  }
}


/*
   show duration of a down-up press
*/
void print_down_up_duration() {
  if (touch_state == TS_RELEASED) {
    Serial.println(String(millis() * 0.001, 3) + " " + String((millis() - time_pressed) * 0.001, 3));
  }
}


void print_non_idle_event(TSPoint p) {
  unsigned long   time_stamp = millis();

  if (touch_state != TS_IDLE) {
    unsigned long time_stamp = millis();
    Serial.print(time_stamp * 0.001, 3);
    Serial.print("   " + String(p.x) + ", " + String(p.y) + ", " + String(p.z));
    Serial.print("   " + String(touch_pressure) + "  " + String(touch_pressed));
    Serial.print("   " + String(touch_state_previous) + "  " + String(touch_state));
    if (touch_state == TS_PRESSED) Serial.print("   TS_PRESSED");
    else if (touch_state == TS_DOWN) Serial.print("   TS_DOWN");
    else if (touch_state == TS_FLICKER) Serial.print("   TS_FLICKER");
    else if (touch_state == TS_RELEASED) Serial.print("   TS_RELEASED");
    Serial.println();
  }
}
