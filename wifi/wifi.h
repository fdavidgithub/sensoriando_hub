#ifndef WIFILIB
#define WIFILIB
/*
 * 
 */
#include <ESP8266WiFi.h> 
#include <SimpleEspNowConnection.h>

/*
 * MACROS
 */
#define DEBUG

/*
 * GlobalVariables
 */
static SimpleEspNowConnection simpleEspConnection(SimpleEspNowRole::SERVER);
String inputString, clientAddress;
SensoriandoSensorDatum myData;

void OnSendError(uint8_t* ad)
{
#ifdef DEBUG
Serial.println("Sending to '"+simpleEspConnection.macToStr(ad)+"' was not possible!");  
#endif
}

void OnMessage(uint8_t* ad, uint8_t* message, uint8_t len)
{
  memcpy(&myData, message, sizeof(myData));

#ifdef DEBUG
Serial.print("From client: ");Serial.println(simpleEspConnection.macToStr(ad));
Serial.print("Bytes received: ");Serial.println(len, DEC);
Serial.print("STX: ");Serial.println(myData.stx, HEX);
Serial.print("id: ");Serial.println(myData.id, DEC);
Serial.print("value: ");Serial.println(myData.value, DEC);
Serial.print("ETX: ");Serial.println(myData.etx, HEX);
Serial.println();
#endif
}

void OnPaired(uint8_t *ga, String ad)
{
#ifdef DEBUG
Serial.println("EspNowConnection : Client '"+ad+"' paired! ");
#endif

  simpleEspConnection.endPairing();  
}

void OnConnected(uint8_t *ga, String ad)
{
#ifdef DEBUG
Serial.println(">> ");Serial.print(ad);
Serial.println(">>[CLIENT] connected!");
#endif
}

int wifi_init() 
{
  simpleEspConnection.begin();

  simpleEspConnection.setPairingBlinkPort(2);
//  simpleEspConnection.onMessage(&OnMessage);  
  simpleEspConnection.onPaired(&OnPaired);  
  simpleEspConnection.onSendError(&OnSendError);  
  simpleEspConnection.onConnected(&OnConnected);  

#ifdef DEBUG
Serial.println("Server: ");Serial.print(WiFi.macAddress());
#endif

  return 1;
}

#endif
