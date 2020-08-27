/* 
 * ESP_NOW for Arduino MEGA
 */
#ifndef WIFI_H
#define WIFI_H

#include <sensoriandoData.h>

/*
 * MACROS
 */
#define DEBUG

#define TIMEOUT 5000

/* 
 * GlobalVariables
 */


/*
 * Prototypes
 */
byte wifi_init();

 
/*
 * functions
 */
byte wifi_init() {
    SensoriandoWifiCommand mycmd;
    long timeelapsed;
    byte stream;
    
    Serial1.begin(115200);

    Serial1.write(SYN);
    Serial1.write(SYN);
    Serial1.write(STX);
    Serial1.write(CMD_INIT);
    Serial1.write(ETX);
    delay(1);

    mycmd.stx = NULL;
    mycmd.cmd = NULL;
    mycmd.etx = NULL;
    
    timeelapsed = millis();   
    while ( (millis() - timeelapsed) < TIMEOUT) {
      if ( Serial1.available() ) {
          stream = Serial1.read();

          switch ( stream ) {
            case SYN: break;
            case STX: mycmd.stx = STX;
                      break;
            case ETX: mycmd.etx = ETX;
                      break;
            default: mycmd.cmd = stream;
          }
      }

      if ( mycmd.etx == ETX ) {
#ifdef DEBUG
Serial.println(mycmd.stx, HEX);
Serial.println(mycmd.cmd, HEX);
Serial.println(mycmd.etx, HEX);
#endif
        break;
      }

      delay(1);
    }

    return (mycmd.cmd == ACK) && (mycmd.stx == STX) && (mycmd.etx == ETX);
}

#endif
