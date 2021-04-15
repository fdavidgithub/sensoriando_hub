/*
  SensorServer
  
  Basic EspNowConnection Server implementation for a sensor server (gateway)

  HOWTO Arduino IDE:
  - Prepare two ESP8266 based devices (eg. WeMos)
  - Start two separate instances of Arduino IDE and load 
    on the first one the 'SensorServer.ino' and
    on the second one the 'SensorClientDigitalInput.ino' sketch and upload 
    these to the two ESP devices.
  - Start the 'Serial Monitor' in both instances and set baud rate to 9600
  - Type 'startpair' into the edit box of the server. Hold the pairing button on the sensor and reset the device
  - After server and client are paired, you can change the sleep time of the client in the server
    by typing 'settimeout <seconds>' into the serial terminal. 
    This will than be send next time when sensor is up.
  
  - You can use multiple clients which can be connected to one server

  Created 11 Mai 2020
  By Erich O. Pintar
  Modified 19 Jun 2020
  By Erich O. Pintar

  https://github.com/saghonfly/SimpleEspNowConnection

*/
#include <sensoriando.h>
#include <SimpleEspNowConnection.h>
//#include "gpio.h"

#define GPIO_PAIR 0

static SimpleEspNowConnection simpleEspConnection(SimpleEspNowRole::SERVER);
String inputString, clientAddress;

SensoriandoSensorDatum myData;

/*
 * Prototypes
 */
//void OnSendError(uint8_t*);
void OnMessage(uint8_t*, const uint8_t*, size_t);
void OnPaired(uint8_t *, String);
void OnConnected(uint8_t *, String);

void setup() 
{
  Serial.begin(115200);
  pinMode(GPIO_PAIR, INPUT);
  
  simpleEspConnection.begin();
  simpleEspConnection.setPairingBlinkPort(LED_BUILTIN);
  simpleEspConnection.onMessage(&OnMessage);  
  simpleEspConnection.onPaired(&OnPaired);  
//  simpleEspConnection.onSendError(&OnSendError);  
  simpleEspConnection.onConnected(&OnConnected);  

  Serial.println("Server: ");
  Serial.print(WiFi.macAddress());   
}

void loop() 
{
  if ( Serial.available() && (Serial.read() == 'r') ) {
    ESP.reset();
  }

  simpleEspConnection.loop();
  
  if ( !digitalRead(GPIO_PAIR) ) {
    Serial.println("Pairing started...");
    delay(1000);
    
    simpleEspConnection.startPairing(120);  
  }    
}

/*
 * functions
 */
/* 
void OnSendError(uint8_t* ad)
{
  Serial.println("Sending to '"+simpleEspConnection.macToStr(ad)+"' was not possible!");  
}
*/
void OnMessage(uint8_t* ad, const uint8_t* message, size_t len)
{
  memcpy(&myData, message, sizeof(myData));

  Serial.print("From client: ");Serial.println(simpleEspConnection.macToStr(ad));
  Serial.print("Bytes received: ");Serial.println(len, DEC);
  Serial.print("STX: ");Serial.println(myData.stx, HEX);
  Serial.print("id: ");Serial.println(myData.id, DEC);
  Serial.print("value: ");Serial.println(myData.value, DEC);
  Serial.print("ETX: ");Serial.println(myData.etx, HEX);
  Serial.println();

}

void OnPaired(uint8_t *ga, String ad)
{
  Serial.println("EspNowConnection : Client '"+ad+"' paired! ");

  simpleEspConnection.endPairing();  
}

void OnConnected(uint8_t *ga, String ad)
{
    Serial.println(">> ");Serial.print(ad);
    Serial.println(">>[CLIENT] connected!");
}
 
