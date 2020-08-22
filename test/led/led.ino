#include "gpio.h"

void setup() {
  // put your setup code here, to run once:
  pinMode(GPIO_RED, OUTPUT);
  pinMode(GPIO_GREEN, OUTPUT);
  pinMode(GPIO_BLUE, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(GPIO_RED, 255);
  digitalWrite(GPIO_GREEN, 0);
  digitalWrite(GPIO_BLUE, 0);
  delay(1000);
  
  digitalWrite(GPIO_RED, 0);
  digitalWrite(GPIO_GREEN, 255);
  digitalWrite(GPIO_BLUE, 0);
  delay(1000);

  digitalWrite(GPIO_RED, 0);
  digitalWrite(GPIO_GREEN, 0);
  digitalWrite(GPIO_BLUE, 255);
  delay(1000);
  
}
