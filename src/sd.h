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
float sd_fullsize();
float sd_freespace();
byte sd_readdatum(SensoriandoSensorDatum *);
void sd_dropdb();

#endif
