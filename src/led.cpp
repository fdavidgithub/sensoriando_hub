#include "led.h"

long led_modeerror() {
    analogWrite(GPIO_RED, 255);
    analogWrite(GPIO_GREEN, 0);
    analogWrite(GPIO_BLUE, 0);

    return millis();
}

long led_modeconfig()
{
    analogWrite(GPIO_RED, 0);
    analogWrite(GPIO_GREEN, 0);
    analogWrite(GPIO_BLUE, 255);

    return millis();
}

void led_modenormal()
{
    analogWrite(GPIO_RED, 0);
    analogWrite(GPIO_GREEN, 0);
    analogWrite(GPIO_BLUE, 0);
}

void led_modesend(long elapsed)
{
    if ( (millis() - elapsed) < MODESEND_UPDATE ) {
        analogWrite(GPIO_RED, 0);
        analogWrite(GPIO_GREEN, 255);
        analogWrite(GPIO_BLUE, 0);
    }
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

