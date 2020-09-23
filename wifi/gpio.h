/*
 * GPIO Map
 */
#ifndef GPIO_H
#define GPIO_H

#ifndef D0
#define GPIO_PAIR   0   //ESP-01, GPIO 0
#else
#define GPIO_PAIR   D0  //NodeMCU, GPIO 16
#endif
 
#define GPIO_LED    LED_BUILTIN

#endif
