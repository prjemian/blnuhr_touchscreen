// blnuhr_rtc.h

#ifndef BLNUHR_RTC
#define BLNUHR_RTC

#include <RtcDS3231.h>
#include <Wire.h>

#define countof(a) (sizeof(a) / sizeof(a[0]))
#define ISO8601_FORMAT          "%04u-%02u-%02uT%02u:%02u:%02u"
#define ISO8601_FORMAT_RELAXED  "%04u-%02u-%02u %02u:%02u:%02u"

void setup_RTC(RtcDS3231<TwoWire> Rtc);
void printDateTime(const RtcDateTime& dt);
void printLabeledTime(char *label, const RtcDateTime& dt);

#endif // BLNUHR_RTC
