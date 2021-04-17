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

#define MESSAGE_LOG   "logsys.txt"
#define DATA_DB       "data.hex"


/*
 * GlobalVariables
 */
//float SdFreeSpace;


/*    
 * Prototypes
 */
byte sd_init();
void sd_writemsg(char *);
void sd_writedatum(SensoriandoSensorDatum *);
byte sd_readdatum(SensoriandoSensorDatum *);
void sd_dropdb();
long sd_usedsize(File, long);
long sd_fullsize();
long sd_freespace(long);
 

#endif
