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
//#define DEBUG_WIFI

#define TIMEOUT 5000

/*
 * Prototypes
 */
byte wifi_init(long);
void wifi_update(long);
byte wifi_available(SensoriandoSensorDatum *); 
void wifi_pair();

#endif
