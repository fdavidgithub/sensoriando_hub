/*
  Basic EspNowConnection Client implementation for a sensor
  BASED https://github.com/saghonfly/SimpleEspNowConnection
*/
#include <sensoriando.h>
#include <SimpleEspNowConnection.h>
//#include "gpio.h"

#define GPIO_PAIR 0

SimpleEspNowConnection SimpleEspConnection(SimpleEspNowRole::CLIENT);
SensoriandoSensorDatum myData;

String ServerAddress;
int CounterSent=0;
int CounterError=0;
int Paired=0;

/*
 * Prototypes
 */
void espnow_init();
bool readConfig();
bool writeConfig();
void readSensor();
void OnNewGatewayAddress(uint8_t *, String);
void OnSendError(uint8_t*);


void setup()
{
  Serial.begin(115200);
  pinMode(GPIO_PAIR, INPUT);

  // start SPIFFS file system. Ensure, sketch is uploaded with FS support !!!
  if (!SPIFFS.begin()) {
    Serial.print("FS formating...");
    SPIFFS.format(); Serial.println("OK");
  }
  
  SimpleEspConnection.begin();
  SimpleEspConnection.setPairingBlinkPort(2); 
  SimpleEspConnection.onNewGatewayAddress(&OnNewGatewayAddress);

  Serial.print("Client Address: ");Serial.println(WiFi.macAddress());
  espnow_init();
}

void loop()
{
  if ( Serial.available() ) {
    switch ( Serial.read() ) {
      case 'r': Serial.print("Resetting...");ESP.reset(); break;
      case 'f': Serial.print("Formating...");SPIFFS.format();Serial.println("DONE");break;
    }
  }
 

  SimpleEspConnection.loop();

  if ( !digitalRead(GPIO_PAIR) ) {
    Serial.println("Pairing started...");
    delay(1000);
    SimpleEspConnection.startPairing(120);
  }
  
  if ( Paired ) {
    Serial.println("Sending datum...");
    readSensor();
    
    if ( SimpleEspConnection.sendMessage((uint8_t *)&myData, sizeof(myData)) ) {
      CounterSent++;
      Serial.println("OK");  
    } else {
      Serial.println("FAIL");
    }

    Serial.print(" | Sent: ");Serial.print(CounterSent);
    Serial.print(", error: ");Serial.print(CounterError);
    Serial.println("");
  }

  delay(1000);
}

/*
 * functions 
 */
void espnow_init() 
{
  if (!readConfig())
  {
    Serial.println("!!! [READ] Server address not save. Please pair first !!!");
  } else {
      Serial.print("Server address saved: ");Serial.println(ServerAddress);

      if (!SimpleEspConnection.setServerMac(ServerAddress)) // set the server address which is stored in EEPROM
      {
        Serial.println("!!! [CONNECT] Server address not valid. Please pair again !!!");
      } else {
        Serial.print("Connected on Server Address:");Serial.println(ServerAddress);
        Paired = 1;
      }  
  }


}

void OnSendError(uint8_t* ad)
{
  Serial.println("Sending to " + SimpleEspConnection.macToStr(ad) + " was not possible!");
  CounterError++;
}


bool readConfig()
{
  if (!SPIFFS.exists("/setup.txt"))
    return false;

  File configFile = SPIFFS.open("/setup.txt", "r");
  if (!configFile)
    return false;

  for (int i = 0; i < 12; i++) //Read server address
    ServerAddress += (char)configFile.read();

  configFile.close();
  return true;
}

bool writeConfig()
{
  File configFile = SPIFFS.open("/setup.txt", "w");
  if (!configFile)
    return false;

  configFile.print(ServerAddress.c_str());
  configFile.close();

  return true;
}

void OnNewGatewayAddress(uint8_t *ga, String ad)
{
  ServerAddress = ad;
  SimpleEspConnection.setServerMac(ga);

  Serial.println("Pairing mode finished...");
  Serial.println("EspNowConnection : Server '" + ad + " paired! ");

  writeConfig();
  ESP.reset();
}

void readSensor() 
{
  myData.stx = 0x02;
  myData.id = 42;
  myData.dt = NULL;
  myData.value = random(1,100)/PI;  
  myData.etx = 0x03;

  Serial.print("Bytes sent: ");Serial.println(sizeof(myData));
  Serial.printf("stx=0x%02X, id=%d, value=%02f, etx=0x%02X\n", myData.stx, myData.id, myData.value, myData.etx);
}
 
