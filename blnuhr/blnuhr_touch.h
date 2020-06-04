// blnuhr_touch.h

// uses: Adafruit Touchscreen 1.1.0

#ifndef BLNUHR_TOUCH
#define BLNUHR_TOUCH

#include <TouchScreen.h>

// touch panel constants 
// calibrated for Velleman VMA412 with ID=0x9341
#define XP  8
#define XM A2
#define YP A3
#define YM  9
// touch coordinates
#define TS_LEFT 914
#define TS_RT   122
#define TS_TOP  85
#define TS_BOT  908

#define TS_MINPRESSURE 50
#define TS_MAXPRESSURE 1000

// custom touch event support

// average the touch pad pressure signal, it's noisy
// this float ranges from 0 .. 1 and is the fraction of the old to keep
#define TOUCH_SMOOTHING_FACTOR 0.7
#define SMOOTH(avg, new_value) (TOUCH_SMOOTHING_FACTOR*avg + (1-TOUCH_SMOOTHING_FACTOR)*new_value)

enum ts_touch_states_enum {TS_IDLE, TS_PRESSED, TS_DOWN, TS_FLICKER, TS_RELEASED};
// TS_IDLE      no touch screen event at this time
// TS_PRESSED   touch screen was just pressed
// TS_DOWN      touch screen still pressed
// TS_FLICKER   touch screen pressure is fluctuating
// TS_RELEASED  touch screen was just released
// CAUTION: watch for TS_RELEASED -> TS_PRESSED transitions (with no intervening TS_IDLE, could be more flicker)

#define TS_FLICKER_DELAY_MS 200

TSPoint getTouchPoint();
void ts2pixel(TSPoint p, int *x, int *y);
void print_down_up_duration();
void print_non_idle_event(TSPoint p);

#endif // BLNUHR_TOUCH
