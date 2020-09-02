#ifndef RTC_H
#define RTC_H
/*
 * Date and time functions using a DS1307 RTC connected via I2C and Wire lib
 */
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
 * GlobalVariables
 */
Nanoshield_RTC rtcclient;

/*
 * Prototypes
 */
byte rtc_init();
DateTime rtc_get();
byte rtc_check();
 
/*
 * functions
 */
byte rtc_init () { 
  byte res;

  #ifdef ESP8266
      Wire.begin(GPIO_SDA, GPIO_SCL);
  #endif
  
#ifdef DEBUG_RTC
Serial.println("GPIO SDA: ");Serial.print(GPIO_SDA);Serial.print(" | SCL: ");Serial.print(GPIO_SCL);
#endif

  res = rtcclient.begin();
  return res;
}

DateTime rtc_get () {  
    DateTime dt;
    
    rtcclient.read();
    dt = DateTime(rtcclient.getYear(), rtcclient.getMonth(), rtcclient.getDay(), rtcclient.getHours(), rtcclient.getMinutes(), rtcclient.getSeconds()); 
    
#ifdef DEBUG_RTC
Serial.print("Unixtime: ");Serial.println(dt.unixtime());
#endif
    
    return dt; 
}

void rtc_sync(DateTime dt_ntp, DateTime dt_rtc) {
    rtcclient.writeYear(dt_ntp.year());
    rtcclient.writeMonth(dt_ntp.month());
    rtcclient.writeDay(dt_ntp.day());
    rtcclient.writeHours(dt_ntp.hour());
    rtcclient.writeMinutes(dt_ntp.minute());
    rtcclient.writeSeconds(dt_ntp.second());
    
    delay(100);
}

byte rtc_check() {
  return rtcclient.getDay() != 0;  
}

#endif
