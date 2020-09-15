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

#define MODESEND_UPDATE 300000  //Blink mode send in miliseconds
                                //5 min = 300 s = 300000 ms


/*    
 * Prototypes
 */
long led_modeerror();
long led_modeconfig();
void led_modenormal();
void led_modesend(long);
void led_init();

#endif
