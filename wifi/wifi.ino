/*
 * Build with Arduino 1.8.12
 * Board NodeMCU 1.0 (ESP-12E Module)
 * 
 */
#include <sensoriandoData.h>
#include <SimpleEspNowConnection.h>
#include <time.h>
#include "gpio.h"


/* 
 *  MACROS
 */
//#define DEBUG

#define DEBOUNCE  500
#define TIMEOUT   1000
#define MSTOS     1000 //Milli seconds in seconds
/*
 * GlobalVariables
 */
static SimpleEspNowConnection simpleEspConnection(SimpleEspNowRole::SERVER);
String inputString, clientAddress;

SensoriandoSensorDatum myData;

#ifdef DEBUG
  time_t DtNow = 1599612713;
#else
  time_t DtNow = NULL;
#endif

long timeelapsed;


/*
 * Prototypes
 */
void OnSendError(uint8_t*);
void OnMessage(uint8_t*, const uint8_t*, size_t);
void OnPaired(uint8_t *, String);
void OnConnected(uint8_t *, String);
void CmdInit(SensoriandoWifiCommandInit *);
void SerialFlush();


/*
 * Main
 */
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

  timeelapsed = millis();
}

void loop() 
{
  SensoriandoWifiCommandInit cmdinit = {NULL, NULL, NULL, NULL};
  byte stream = NULL;
  byte bufstream[sizeof(cmdinit)] = {};
  
  simpleEspConnection.loop();
  
  if ( (DtNow != NULL) && ((millis() - timeelapsed) >= MSTOS) ) {
      DtNow++; 
      timeelapsed = millis(); 

#ifdef DEBUG
struct tm  ts;
char       buf[80];
ts = *localtime(&DtNow);
strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
Serial.printf("%s\n", buf);
#endif
  }
  
  if ( Serial.available() ) {
    digitalWrite(GPIO_LED, 0);
    
    Serial.readBytes(bufstream, sizeof(bufstream));
    delay(1);

    memcpy(&cmdinit, bufstream, sizeof(bufstream));
    
#ifdef DEBUG
Serial.print("[CMD] Reading (bytes): ");Serial.println(sizeof(cmdinit), DEC);
Serial.print("STX: ");Serial.println(cmdinit.stx, HEX);
Serial.print("cmd: ");Serial.println(cmdinit.cmd, HEX);
Serial.print("dt: ");Serial.println(cmdinit.param, DEC);
Serial.print("ETX: ");Serial.println(cmdinit.etx, HEX);
#endif

    if ( (cmdinit.stx == STX) && (cmdinit.etx == ETX) ) {
#ifdef DEBUG
Serial.print("Incomme command: 0x");Serial.println(cmdinit.cmd, HEX);
#endif 
        switch ( cmdinit.cmd ) {
          case CMD_INIT: CmdInit(&cmdinit);
                         break;   
          default: 
#ifdef DEBUG
Serial.println("WHATTT???!!!!");
#endif           
                break;
        }
    } else {
#ifdef DEBUG
Serial.println("Incomme serial: Nothing");
#endif        
    }

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
void SerialFlush()
{
  while ( Serial.available()  ) {
    Serial.read();    
    delay(1);
  }
}

void CmdInit(SensoriandoWifiCommandInit *cmdinit)
{ 
  SensoriandoWifiCommandResult cmdresult;

#ifdef DEBUG
Serial.println("Command**********");
Serial.print("STX: ");Serial.println(cmdinit->stx, HEX);
Serial.print("cmd: ");Serial.println(cmdinit->cmd, HEX);
Serial.print("dt: ");Serial.println(cmdinit->param, DEC);
Serial.print("ETX: ");Serial.println(cmdinit->etx, HEX);
Serial.println();
#endif

  cmdresult.stx = STX;
  cmdresult.etx = ETX;
  cmdresult.res = NAK;
  cmdresult.cmd = cmdinit->cmd;
  
  if ( cmdinit->cmd = CMD_INIT) {
      DtNow = cmdinit->param;
      timeelapsed = millis();
      cmdresult.res = ACK;    
  }
  
#ifdef DEBUG
Serial.println("Result************");
Serial.print("STX: ");Serial.println(cmdresult.stx, HEX);
Serial.print("cmd: ");Serial.println(cmdresult.cmd, HEX);
Serial.print("result: ");Serial.println(cmdresult.res, DEC);
Serial.print("ETX: ");Serial.println(cmdresult.etx, HEX);
Serial.println();
#endif    

  SerialFlush();
  
  Serial.write((uint8_t *)&cmdresult, sizeof(cmdresult));
  delay(1);
}

void OnSendError(uint8_t* ad)
{
  Serial.println("Sending to '"+simpleEspConnection.macToStr(ad)+"' was not possible!");  
}

void OnMessage(uint8_t* ad, const uint8_t* message, size_t len)
{
  digitalWrite(GPIO_LED, 0);
  
  memcpy(&myData, message, sizeof(myData));
  myData.dt = DtNow;

#ifdef DEBUG
Serial.print("From client: ");Serial.println(simpleEspConnection.macToStr(ad));
Serial.print("Bytes received: ");Serial.println(len, DEC);
Serial.print("STX: ");Serial.println(myData.stx, HEX);
Serial.print("id: ");Serial.println(myData.id, DEC);
Serial.print("value: ");Serial.println(myData.value, DEC);
Serial.print("dt: ");Serial.println(myData.dt, DEC);
Serial.print("ETX: ");Serial.println(myData.etx, HEX);
Serial.println();
#endif

  if (myData.dt != NULL) {
      Serial.write((uint8_t *)&myData, sizeof(myData));
  } else {
#ifdef DEBUG
Serial.println("Do not init!!!");
#endif     
  }
  
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
 
