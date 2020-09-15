/* 
 * ESP_NOW for Arduino MEGA
 */
#ifndef WIFI_H
#define WIFI_H

#include <Arduino.h>
#include <sensoriandoData.h>
#include <time.h>

/*
 * MACROS
 */
#define DEBUG_WIFI

#define TIMEOUT 5000

/* 
 * GlobalVariables
 */


/*
 * Prototypes
 */
byte wifi_init(long);
byte wifi_available(SensoriandoSensorDatum *); 
 
#endif