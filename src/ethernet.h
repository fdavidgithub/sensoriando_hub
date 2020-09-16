/*
 * 
 */
#ifndef ETHERNET_H
#define ETHERNET_H

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Nanoshield_Ethernet.h>
#include <Nanoshield_EEPROM.h>


/*
 * MACROS
 */
//#define DEBUG_ETH


/*
 * Prototypes
 */
byte ethernet_init();

#endif
