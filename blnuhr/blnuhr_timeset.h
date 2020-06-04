// blnuhr_timeset.h

#ifndef BLNUHR_TIMESET
#define BLNUHR_TIMESET

#include <Fonts/FreeSansBold12pt7b.h>

// synchronize with buttons defined in blnuhr_timeset.ino
enum button_names_enum {
    BTN_CANCEL = 0, BTN_OK, 
    BTN_H12_UP, BTN_H12_DN, 
    BTN_H1_UP,  BTN_H1_DN, 
    BTN_M10_UP, BTN_M10_DN, 
    BTN_M1_UP,  BTN_M1_DN,
    NO_BUTTON
};

#define MINUTE_ONE  60L;
#define MINUTE_TEN  10*MINUTE_ONE;
#define HOUR_ONE    6*MINUTE_TEN;
#define HOUR_TWELVE 12*HOUR_ONE;

void start_timeset(RtcDateTime the_time);
bool loop_timeset(TSPoint p);

// internal
void makeSpinButton(Adafruit_GFX_Button *btn, int x, int y, int digit);
int getButtonIndex(int x, int y, Adafruit_GFX_Button **pb);
bool process_button_press(int button_index);

#endif // BLNUHR_TIMESET
