#ifndef RTC_H
#define RTC_H
/*
 * Date and time functions using a DS1307 RTC connected via I2C and Wire lib
 */
#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include <Nanoshield_RTC.h>

#ifndef GPIO_H
  #include "gpio.h"
#endif

/*
 * MACROS
 */
//#define DEBUG_RTC


/*
 * Prototypes
 */
byte rtc_init(Nanoshield_RTC *);
DateTime rtc_get(Nanoshield_RTC *);
byte rtc_check(Nanoshield_RTC *);
void rtc_sync(Nanoshield_RTC *, DateTime, DateTime);


#endif
