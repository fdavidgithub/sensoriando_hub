#ifndef NTP_H
#define NTP_H
/*
 * 
 */
 #include <RTClib.h>
#include <NTPClient.h>
#include <EthernetUdp.h>

/*
 * MACROS
 */
//#define DEBUG
#define TIMEZONE        0   //UTC

/*
 * functions
 */
DateTime ntp_get() 
{
  EthernetUDP ntpUDP;
  DateTime ntp = NULL;

  NTPClient timeClient(ntpUDP, "pool.ntp.br", TIMEZONE*3600, 60000);

  timeClient.begin();

  if ( timeClient.update() ) { 
#ifdef DEBUG    
      Serial.print("NTP: ");
      Serial.print(timeClient.getFormattedTime());
#endif

      ntp = DateTime(timeClient.getEpochTime());
  }
      
  timeClient.end();
  
  return ntp;
}
 
#endif
