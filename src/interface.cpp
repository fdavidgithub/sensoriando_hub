#include "interface.h"

byte interface_pair() 
{
    int button = analogRead(GPIO_BUTTONS);

#ifdef DEBUG_INTERFACE
if (button < 69) {
Serial.println("[INTERFACE] button pairing pressed");
}
#endif

    return button < 69; //Button RIGHT
}

long interface_modeerror() 
{
    analogWrite(GPIO_YELLOW, 255);
    analogWrite(GPIO_GREEN, 255);

    return millis();
}

long interface_modeconfig()
{
    analogWrite(GPIO_YELLOW, 255);
    return millis();
}

void interface_modenormal()
{
    analogWrite(GPIO_YELLOW, 0);
    analogWrite(GPIO_GREEN, 0);
}

void interface_modesend(long elapsed)
{
    if ( (millis() - elapsed) < MODESEND_UPDATE ) {
        analogWrite(GPIO_GREEN, 255);
    }
} 

void interface_init()
{
    pinMode(GPIO_YELLOW, OUTPUT);
    pinMode(GPIO_GREEN, OUTPUT);
    pinMode(GPIO_BUTTONS, INPUT);

#ifdef DEBUG_INTERFACE
interface_modeerror;delay(1000);interface_modenormal;delay(1000);
interface_modesend(millis());delay(1000);interface_modeconfig();delay(1000);
#endif

    interface_modenormal();
}

