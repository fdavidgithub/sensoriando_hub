/*
 * 
 */
#ifndef LED_H
#define LED_H

#ifndef GPIO_H
  #include "gpio.h"
#endif

#include "Arduino.h"


/*
 * MACROS
 */
//#define DEBUG_LED


/*    
 * Prototypes
 */
void led_modeerror();
void led_modeconfig();
void led_modenormal();
void led_modesend();
void led_init();

#endif
