/*
   Build with Arduino 1.8.12
   Board NodeMCU 1.0 (ESP-12E Module)

*/
#include <sensoriandoData.h>
#include <SimpleEspNowConnection.h>
#include <time.h>
#include "gpio.h"


/*
    MACROS
*/
//#define DEBUG

#define TIMEOUT   500   //Milli seconds
#define TIMEPAIR  120   //seconds
#define MSTOS     1000  //Milli seconds in seconds


/*
   GlobalVariables
*/
static SimpleEspNowConnection simpleEspConnection(SimpleEspNowRole::SERVER);
String inputString, clientAddress;
SensoriandoSensorDatum myData;
time_t DtNow = NULL;
long timeelapsed;


/*
   Prototypes
*/
//void OnSendError(uint8_t*);
void OnMessage(uint8_t*, const uint8_t*, size_t);
void OnPaired(uint8_t *, String);
void OnConnected(uint8_t *, String);

void CmdInit(SensoriandoWifiCommandInit *);
void CmdUpdate(SensoriandoWifiCommandInit *);
void CmdPair();


/*
   Main
*/
void setup()
{
  Serial.begin(115200);

  simpleEspConnection.begin();
  simpleEspConnection.setPairingBlinkPort(GPIO_LED);
  
  simpleEspConnection.onMessage(&OnMessage);
  simpleEspConnection.onPaired(&OnPaired);
//  simpleEspConnection.onSendError(&OnSendError);
  simpleEspConnection.onConnected(&OnConnected);

#ifdef DEBUG
  Serial.println();
  Serial.print("[Server] MAC ADDRESS: "); Serial.println(WiFi.macAddress());
  Serial.print("Data struct (bytes): "); Serial.println(sizeof(SensoriandoSensorDatum));
  Serial.print("Comand struct (bytes): "); Serial.println(sizeof(SensoriandoWifiCommandInit));
#endif

  timeelapsed = millis();
}

void loop()
{
  SensoriandoWifiCommandInit cmdinit = {NULL, NULL, NULL, NULL};
  char stream;
  byte bufstream[sizeof(cmdinit)] = {};
  long elapsedtime_serial;

  simpleEspConnection.loop();

  if ( (DtNow != NULL) && ((millis() - timeelapsed) >= MSTOS) ) {
    DtNow++;
    timeelapsed = millis();

#ifdef DEBUG
    struct tm  ts;
    char buf[80];
    ts = *localtime(&DtNow);
    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
    Serial.print(DtNow); Serial.printf(" | %s\n", buf);
#endif
  }

  if ( Serial.available() ) {
    digitalWrite(GPIO_LED, 0);
    elapsedtime_serial = millis();
    stream = Serial.read();

#ifdef DEBUG
    stream = SYN;
#endif

    while ( (stream == SYN) && (millis() - elapsedtime_serial) <= TIMEOUT ) {
      if ( Serial.available() ) {
        Serial.readBytes(bufstream, sizeof(bufstream));
        memcpy(&cmdinit, bufstream, sizeof(bufstream));

#ifdef DEBUG
        cmdinit.stx = STX;
        cmdinit.cmd = CMD_PAIR;
        cmdinit.param = NULL;
        cmdinit.etx = ETX;

        Serial.print("[CMD] Reading (bytes): "); Serial.println(sizeof(cmdinit), DEC);
        Serial.print("STX: "); Serial.println(cmdinit.stx, HEX);
        Serial.print("cmd: "); Serial.println(cmdinit.cmd, HEX);
        Serial.print("dt: "); Serial.println(cmdinit.param, DEC);
        Serial.print("ETX: "); Serial.println(cmdinit.etx, HEX);
#endif

        if ( (cmdinit.stx == STX) && (cmdinit.etx == ETX) ) {
#ifdef DEBUG
          Serial.print("Incomme command: 0x"); Serial.println(cmdinit.cmd, HEX);
#endif
          switch ( cmdinit.cmd ) {
            case CMD_INIT:  CmdInit(&cmdinit);
              break;
            case CMD_UPD:   CmdUpdate(&cmdinit);
              break;
            case CMD_PAIR:  CmdPair();
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

        break; //Exit while because do not anything to do
      }
    }

    digitalWrite(GPIO_LED, 1);
  }
}


/*
   functions
*/
void CmdUpdate(SensoriandoWifiCommandInit *cmdinit)
{
#ifdef DEBUG
  Serial.println("Command**********");
  Serial.print("STX: "); Serial.println(cmdinit->stx, HEX);
  Serial.print("cmd: "); Serial.println(cmdinit->cmd, HEX);
  Serial.print("dt: "); Serial.println(cmdinit->param, DEC);
  Serial.print("ETX: "); Serial.println(cmdinit->etx, HEX);
  Serial.println();
#endif

  if ( cmdinit->cmd = CMD_UPD) {
    DtNow = cmdinit->param;
    timeelapsed = millis();
  }
}

void CmdPair()
{
#ifdef DEBUG
  Serial.println("Pairing started...");
#endif
  simpleEspConnection.startPairing(TIMEPAIR);
}

void CmdInit(SensoriandoWifiCommandInit *cmdinit)
{
  SensoriandoWifiCommandResult cmdresult;

#ifdef DEBUG
  Serial.println("Command**********");
  Serial.print("STX: "); Serial.println(cmdinit->stx, HEX);
  Serial.print("cmd: "); Serial.println(cmdinit->cmd, HEX);
  Serial.print("dt: "); Serial.println(cmdinit->param, DEC);
  Serial.print("ETX: "); Serial.println(cmdinit->etx, HEX);
  Serial.println();
#endif

  cmdresult.stx = STX;
  cmdresult.etx = ETX;
  cmdresult.res = NAK;
  cmdresult.cmd = cmdinit->cmd;

  if ( cmdinit->cmd == CMD_INIT) {
    cmdresult.res = ACK;

    Serial.write(SYN);
    Serial.write((uint8_t *)&cmdresult, sizeof(cmdresult));

#ifdef DEBUG
    Serial.println("Result************");
    Serial.print("STX: "); Serial.println(cmdresult.stx, HEX);
    Serial.print("cmd: "); Serial.println(cmdresult.cmd, HEX);
    Serial.print("result: "); Serial.println(cmdresult.res, DEC);
    Serial.print("ETX: "); Serial.println(cmdresult.etx, HEX);
    Serial.println();
#endif

    DtNow = cmdinit->param; //Variable setted as init
    timeelapsed = millis();
  }
}
/*
void OnSendError(uint8_t* ad)
{
  Serial.println("Sending to '" + simpleEspConnection.macToStr(ad) + "' was not possible!");
}
*/
void OnMessage(uint8_t* ad, const uint8_t* message, size_t len)
{
  memcpy(&myData, message, sizeof(myData));
  myData.dt = DtNow;

#ifdef DEBUG
  Serial.println(""); Serial.println("[OnMessage]");
  Serial.print("From client: "); Serial.println(simpleEspConnection.macToStr(ad));
  Serial.print("Bytes received: "); Serial.println(len, DEC);
  Serial.print("STX: "); Serial.println(myData.stx, HEX);
  Serial.print("UUID: "); Serial.println(myData.uuid);
  Serial.print("id: "); Serial.println(myData.id, DEC);
  Serial.print("value: "); Serial.println(myData.value, DEC);
  Serial.print("dt: "); Serial.println(myData.dt, DEC);
  Serial.print("ETX: "); Serial.println(myData.etx, HEX);
#endif

  if (myData.dt != NULL) {
    digitalWrite(GPIO_LED, 0);
    Serial.write(SYN);
    Serial.write((uint8_t *)&myData, sizeof(myData));
    digitalWrite(GPIO_LED, 1);
  } else {

#ifdef DEBUG
    Serial.println(">> Do not init!!!");
#endif

  }
}

void OnPaired(uint8_t *ga, String ad)
{

#ifdef DEBUG
  Serial.println("EspNowConnection : Client '" + ad + "' paired! ");
#endif

  simpleEspConnection.endPairing();
}

void OnConnected(uint8_t *ga, String ad)
{

#ifdef DEBUG
  Serial.println(">> "); Serial.print(ad);
  Serial.println(">>[CLIENT] connected!");
#endif

}
