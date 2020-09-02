/*
 * Build with Arduino 1.8.12
 * Board NodeMCU 1.0 (ESP-12E Module)
 * 
 */
#include <sensoriandoData.h>
#include <SimpleEspNowConnection.h>
#include "gpio.h"


/* 
 *  MACROS
 */
//#define DEBUG

#define DEBOUNCE  500
#define TIMEOUT   1000

/*
 * GlobalVariables
 */
static SimpleEspNowConnection simpleEspConnection(SimpleEspNowRole::SERVER);
String inputString, clientAddress;

SensoriandoSensorDatum myData;


/*
 * Prototypes
 */
void OnSendError(uint8_t*);
void OnMessage(uint8_t*, const uint8_t*, size_t);
void OnPaired(uint8_t *, String);
void OnConnected(uint8_t *, String);
void Command(SensoriandoWifiCommand*);
void serialflush();

void setup() 
{
  Serial.begin(115200);
  pinMode(GPIO_PAIR, INPUT);
  
  simpleEspConnection.begin();
  simpleEspConnection.setPairingBlinkPort(GPIO_LED);
  simpleEspConnection.onMessage(&OnMessage);  
  simpleEspConnection.onPaired(&OnPaired);  
  simpleEspConnection.onSendError(&OnSendError);  
  simpleEspConnection.onConnected(&OnConnected);  
  
#ifdef DEBUG
Serial.println();
Serial.print("[Server] MAC ADDRESS: ");Serial.println(WiFi.macAddress());   
#endif
}

void loop() 
{
  SensoriandoWifiCommand mycmd;
  long timeelapsed;
  byte stream = NULL;
  
  simpleEspConnection.loop();
  
  mycmd.stx = NULL;
  mycmd.cmd = NULL;
  mycmd.etx = NULL;

  timeelapsed = millis();
  while ( Serial.available() && ((millis() - timeelapsed) < TIMEOUT) ) {
    digitalWrite(GPIO_LED, 0);
    stream = Serial.read();

#ifdef DEBUG
Serial.print(stream, HEX);
#endif

    switch ( stream ) {
        case SYN: break; 
        case STX: mycmd.stx = STX; 
#ifdef DEBUG
Serial.println("stx");
#endif
                  break;
        case ETX: mycmd.etx = ETX; 
#ifdef DEBUG
Serial.println("etx");
#endif
                  break;
        default: mycmd.cmd = stream; 
#ifdef DEBUG
Serial.println("cmd");
#endif
    }

    if ( (mycmd.stx == STX) && (mycmd.etx == ETX) ) {
#ifdef DEBUG
Serial.print("Incomme command: ");Serial.println(mycmd.cmd, HEX);
#endif  
        command(&mycmd);
        serialflush();
        break;
    }

    delay(1);
  }

  if ( stream ) {
    digitalWrite(GPIO_LED, 1);  
  }
      
  if ( digitalRead(GPIO_PAIR) ) {
#ifdef DEBUG
Serial.println("Pairing started...");
#endif

    delay(DEBOUNCE);
    simpleEspConnection.startPairing(120);  
  }
  
}

/*
 * functions
 */
void serialflush()
{
  while ( Serial.available()  ) {
    Serial.read();    
  }
}

void command(SensoriandoWifiCommand* mycmd)
{ 
  switch ( mycmd->cmd ) {
      case CMD_INIT:  mycmd->cmd = ACK;
                      break;
      default: mycmd->cmd = NAK;
  }

  Serial.write(SYN);Serial.write(SYN);
  Serial.write(mycmd->stx);
  Serial.write(mycmd->cmd);
  Serial.write(mycmd->etx);
  
#ifdef DEBUG
Serial.print("Result command: ");Serial.println(mycmd->cmd, HEX);
#endif    
}

void OnSendError(uint8_t* ad)
{
  Serial.println("Sending to '"+simpleEspConnection.macToStr(ad)+"' was not possible!");  
}

void OnMessage(uint8_t* ad, const uint8_t* message, size_t len)
{
  digitalWrite(GPIO_LED, 0);
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

  Serial.write((uint8_t *)&myData, sizeof(myData));
  digitalWrite(GPIO_LED, 1);  
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
 
