/* 
 * ESP_NOW for Arduino MEGA
 */
#ifndef WIFI_H
#define WIFI_H

#include <sensoriandoData.h>
#include <time.h>

/*
 * MACROS
 */
#define DEBUG_WIFI

#define TIMEOUT 5000

/* 
 * GlobalVariables
 */


/*
 * Prototypes
 */
byte wifi_init(long);
byte wifi_available(SensoriandoSensorDatum *); 

 
/*
 * functions
 */
byte wifi_available(SensoriandoSensorDatum *datum) {
    byte stream[sizeof(SensoriandoSensorDatum)];
    
    if ( Serial1.available() ) {
      if ( Serial1.read() == SYN ) {
        delay(1);
        Serial1.readBytes(stream, sizeof(stream));

#ifdef DEBUG_WIFI
Serial.write(stream, sizeof(stream));
#endif
       
        memcpy(datum, stream, sizeof(stream));

#ifdef DEBUG_WIFI
Serial.print("[AVAILABLE] Bytes received: ");Serial.println(sizeof(SensoriandoSensorDatum), DEC);
Serial.print("STX: 0x");Serial.println(datum->stx, HEX);
Serial.print("id: ");Serial.println(datum->id, DEC);
Serial.print("value: ");Serial.println(datum->value, DEC);
Serial.print("dt: ");Serial.println(datum->dt, DEC);
Serial.print("ETX: 0x");Serial.println(datum->etx, HEX);
Serial.println();
#endif
      } else {
#ifdef DEBUG
Serial.println("[AVAILABLE] No Sync");
#endif        
      }
    }

    return (datum->stx == STX) && (datum->etx == ETX);
}

byte wifi_init(long epoch) {
    SensoriandoWifiCommandInit cmdinit;
    SensoriandoWifiCommandResult cmdresult = {NULL, NULL, NULL, NULL};
    long timeelapsed;
    byte bufstream[sizeof(cmdresult)] = {};

    //Send command
    Serial1.begin(115200);

    cmdinit.stx = STX;
    cmdinit.cmd = CMD_INIT;
    cmdinit.param = epoch;
    cmdinit.etx = ETX;

#ifdef DEBUG_WIFI
Serial.print("[CMD INIT] Writing (bytes): ");Serial.println(sizeof(cmdinit), DEC);
Serial.print("STX: 0x");Serial.println(cmdinit.stx, HEX);
Serial.print("cmd: 0x");Serial.println(cmdinit.cmd, HEX);
Serial.print("dt: ");Serial.println(cmdinit.param, DEC);
Serial.print("ETX: 0x");Serial.println(cmdinit.etx, HEX);
Serial.println();
#endif

    Serial1.write(SYN);
    delay(1);
    Serial1.write((uint8_t *)&cmdinit, sizeof(cmdinit));
    
    //Waiting anwser
    timeelapsed = millis();   
    while ( (millis() - timeelapsed) < TIMEOUT) {
      if ( Serial1.available() ) {
        if ( Serial1.read() == SYN ) {
          delay(1);
          Serial1.readBytes(bufstream, sizeof(bufstream));
          
          memcpy(&cmdresult, bufstream, sizeof(bufstream));

#ifdef DEBUG_WIFI
Serial.print("STX: 0x");Serial.println(cmdresult.stx, HEX);
Serial.print("cmd: 0x");Serial.println(cmdresult.cmd, HEX);
Serial.print("result: 0x");Serial.println(cmdresult.res, HEX);
Serial.print("ETX: 0x");Serial.println(cmdresult.etx, HEX);
Serial.println();
#endif

          break;
        } else {
#ifdef DEBUG
Serial.println("[INIT] No Sync");
#endif                  
        }
      }

      delay(1);
    }

    //Valid   
    return (cmdresult.cmd == CMD_INIT) && (cmdresult.res == ACK) && \
           (cmdresult.stx == STX) && (cmdresult.etx == ETX);
}

#endif
