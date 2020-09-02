/*
 * 
 */
#ifndef LED_H
#define LED_H

#ifndef GPIO_H
  #include "gpio.h"
#endif

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

/*
 * functions
 */
void led_modeerror() {
   analogWrite(GPIO_RED, 255);
   analogWrite(GPIO_GREEN, 0);
   analogWrite(GPIO_BLUE, 0);
}

void led_modeconfig()
{
    analogWrite(GPIO_RED, 0);
    analogWrite(GPIO_GREEN, 0);
    analogWrite(GPIO_BLUE, 255);
}

void led_modenormal()
{
    analogWrite(GPIO_RED, 0);
    analogWrite(GPIO_GREEN, 0);
    analogWrite(GPIO_BLUE, 0);
}

void led_modesend()
{
    analogWrite(GPIO_RED, 0);
    analogWrite(GPIO_GREEN, 255);
    analogWrite(GPIO_BLUE, 0);
} 

void led_init()
{
    pinMode(GPIO_RED, OUTPUT);
    pinMode(GPIO_GREEN, OUTPUT);
    pinMode(GPIO_BLUE, OUTPUT);  

#ifdef DEBUG_LED
Serial.println("GPIO RGB r: ");Serial.print(GPIO_RED);Serial.print(" | g: ");Serial.print(GPIO_GREEN);Serial.print(" | b: ");Serial.print(GPIO_BLUE);
delay(1000);led_modeerror;delay(1000);led_modesend();delay(1000);led_modeconfig();delay(1000);led_modenormal();delay(1000);
#endif

    led_modenormal();
}

#endif
