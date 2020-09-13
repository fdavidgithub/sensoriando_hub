#ifndef NTP_H
#define NTP_H
/*
 * 
 */
#include <Arduino.h>
#include <RTClib.h>
#include <NTPClient.h>
#include <EthernetUdp.h>


/*
 * MACROS
 */
//#define DEBUG_NTP
#define TIMEZONE        0   //UTC


/*
 * Prototypes
 */
DateTime ntp_get();
 
#endif
