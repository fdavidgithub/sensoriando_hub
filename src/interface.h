/*
 * 
 */
#ifndef INTERFACE_H
#define INTERFACE_H

#ifndef GPIO_H
  #include "gpio.h"
#endif

#include "Arduino.h"


/*
 * MACROS
 */
#define DEBUG_INTERFACE

#define MODESEND_UPDATE 300000  //Blink mode send in miliseconds
                                //5 min = 300 s = 300000 ms


/*    
 * Prototypes
 */
long interface_modeerror();
long interface_modeconfig();
void interface_modenormal();
void interface_modesend(long);
void interface_init();
byte interface_pair();

#endif
