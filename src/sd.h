/*
 * 
 */
#ifndef SD_H
#define SD_H

#include <Arduino.h>
#include <sensoriando.h>
#include <SPI.h>
#include <SD.h>

#ifndef GPIO_H
  #include "gpio.h"
#endif


/*
 * MACROS
 */
#define DEBUG_SD

#define MESSAGE_LOG     "logsys.txt"
#define DATA_DIR        "/data"
#define LOSTFOUND_DIR   "/lostdata"
#define SENT_DIR        "/sentdata"


/*
 * GlobalVariables
 */
//float SdFreeSpace;


/*    
 * Prototypes
 */
byte sd_init();
void sd_writemsg(char *);
byte sd_writedatum(SensoriandoSensorDatum *);
byte sd_readdatum(SensoriandoSensorDatum *);
long sd_usedsize(File, long);
long sd_fullsize();
long sd_freespace(long);
 

#endif
