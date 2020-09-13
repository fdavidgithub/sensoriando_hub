#include "ntp.h"

DateTime ntp_get() 
{
  EthernetUDP ntpUDP;
  DateTime ntp = NULL;

  NTPClient timeClient(ntpUDP, "pool.ntp.br", TIMEZONE*3600, 60000);

  timeClient.begin();

  if ( timeClient.update() ) { 
#ifdef DEBUG_NTP    
      Serial.print("NTP: ");
      Serial.print(timeClient.getFormattedTime());
#endif

      ntp = DateTime(timeClient.getEpochTime());
  }
      
  timeClient.end();
  
  return ntp;
}
 
