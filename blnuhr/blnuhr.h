/*
 * blnuhr.h
 */

#ifndef BLNUHR
#define BLNUHR

enum display_modes_enum {DISPLAY_CLOCK, DISPLAY_TEMPERATURE, DISPLAY_TIMESET};

#define COMM_SPEED  9600
#define LOOP_DELAY 5

#define COMM_SPEED         9600
#define UPDATE_INTERVAL_MS    5
#define REPORTING_INTERVAL_S 10

#define SHOW_TEMPERATURE_DURATION_MS      10000
#define SHOW_TIMESET_DURATION_MS          60000
#define TRIGGER_TIMESET_PRESS_DURATION_MS  5000
#define TIMEOUT_TIMESET_PRESS_DURATION_MS 10000

void check_touch_in_clock_mode();

#endif // BLNUHR
