#include "wifi.h"

void wifi_pair()
{
    SensoriandoWifiCommandInit cmdinit = {NULL, NULL, NULL, NULL};


    /*
     * Send command
     */
    cmdinit.stx = STX;
    cmdinit.cmd = CMD_PAIR;
    cmdinit.etx = ETX;

#ifdef DEBUG_WIFI
Serial.println("");Serial.print("[Wifi PAIR]");Serial.println((char *)&cmdinit);
Serial.print("[CMD PAIR] Writing (bytes): ");Serial.println(sizeof(cmdinit), DEC);
Serial.print("STX: 0x");Serial.println(cmdinit.stx, HEX);
Serial.print("cmd: 0x");Serial.println(cmdinit.cmd, HEX);
Serial.print("ETX: 0x");Serial.println(cmdinit.etx, HEX);
#endif

    Serial3.write(SYN);
    Serial3.write((uint8_t *)&cmdinit, sizeof(cmdinit));
}

byte wifi_available(SensoriandoSensorDatum *datum)
{
    byte stream[sizeof(SensoriandoSensorDatum)];
    
    if ( Serial3.available() ) {
      if ( Serial3.read() == SYN ) {
        delay(1);
        Serial3.readBytes(stream, sizeof(stream));

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

byte wifi_init(long epoch) 
{
    SensoriandoWifiCommandInit cmdinit;
    SensoriandoWifiCommandResult cmdresult = {NULL, NULL, NULL, NULL};
    long timeelapsed;
    byte bufstream[sizeof(cmdresult)] = {};
    byte stream=NULL;

    Serial3.begin(115200);


    /*
     * Send command
     */
    cmdinit.stx = STX;
    cmdinit.cmd = CMD_INIT;
    cmdinit.param = epoch;
    cmdinit.etx = ETX;

#ifdef DEBUG_WIFI
Serial.println("");Serial.print("[Wifi INIT]");Serial.println((char *)&cmdinit);
Serial.print("[CMD INIT] Writing (bytes): ");Serial.println(sizeof(cmdinit), DEC);
Serial.print("STX: 0x");Serial.println(cmdinit.stx, HEX);
Serial.print("cmd: 0x");Serial.println(cmdinit.cmd, HEX);
Serial.print("dt: ");Serial.println(cmdinit.param, DEC);
Serial.print("ETX: 0x");Serial.println(cmdinit.etx, HEX);
#endif

    Serial3.write(SYN);
    Serial3.write((uint8_t *)&cmdinit, sizeof(cmdinit));


    /*
     * Waiting anwser
     */
    timeelapsed = millis();
    while ( (millis() - timeelapsed) < TIMEOUT) {
#ifdef DEBUG_WIFI
Serial.print(".");
#endif

        if ( Serial3.available() ) {
            switch (stream) {
                case SYN:   Serial3.readBytes(bufstream, sizeof(bufstream));
                            memcpy(&cmdresult, bufstream, sizeof(bufstream));

#ifdef DEBUG_WIFI
Serial.println(">> Anwser");
Serial.print("STX: 0x");Serial.println(cmdresult.stx, HEX);
Serial.print("cmd: 0x");Serial.println(cmdresult.cmd, HEX);
Serial.print("result: 0x");Serial.println(cmdresult.res, HEX);
Serial.print("ETX: 0x");Serial.println(cmdresult.etx, HEX);
Serial.println();
#endif

                            break;
                default:    stream = Serial3.read();
                            break;
            }

            if (cmdresult.res == ACK) {
                break;
            }
        }
    }

    //Valid   
    return (cmdresult.cmd == CMD_INIT) && (cmdresult.res == ACK) && \
           (cmdresult.stx == STX) && (cmdresult.etx == ETX);
}

void wifi_update(long epoch) {
    SensoriandoWifiCommandInit cmdinit;

    cmdinit.stx = STX;
    cmdinit.cmd = CMD_UPD;
    cmdinit.param = epoch;
    cmdinit.etx = ETX;

#ifdef DEBUG_WIFI
Serial.print("[CMD UPD] Writing (bytes): ");Serial.println(sizeof(cmdinit), DEC);
Serial.print("STX: 0x");Serial.println(cmdinit.stx, HEX);
Serial.print("cmd: 0x");Serial.println(cmdinit.cmd, HEX);
Serial.print("dt: ");Serial.println(cmdinit.param, DEC);
Serial.print("ETX: 0x");Serial.println(cmdinit.etx, HEX);
Serial.println();
#endif

    Serial3.write(SYN);
    delay(1);
    Serial3.write((uint8_t *)&cmdinit, sizeof(cmdinit));
    delay(1);    
}

