/* 
 * Receive reads of sensors and publish in MQTT server
 * 
 * Build with IDE Arduino 1.8.12
 * Addicional Board Manager (File >> Preferences)
 * https://dl.espressif.com/dl/package_esp32_index.json
 * http://arduino.esp8266.com/stable/package_esp8266com_index.json
 *
 * REQUIREMENT COMPILE
 * 1x Board NodeMCU
 *    Tools>>Board>>Board Manager (esp8266 by ESP8266 Community v2.7.1)
 *    Tools>>Board (NodeMCU 1.0)
 *      
 * Libraries (Sketch >> Include Library >> Manage Libraies)  
 *    Sensoriando       by Francis David      v1.0.0  
 *    RTCLib            by Adafruit           v1.8.0
 *    NTPClient         by Fabrice Weinberg   v3.2.0
 *    PubSubClient      by Nick O' Leary      v2.7.0
 *    Nanoshield_RTC    by Circuitar          v
 *
 */
#include <sensoriando.h>

#include "src/gpio.h"
#include "src/messages.h"
#include "src/sd.h"
#include "src/rtc.h"
#include "src/ntp.h"
#include "src/interface.h"
#include "src/ethernet.h"
#include "src/wifi.h"


/*
 * MACROS
 */
#define DEBUG

#ifndef DEBUG
    #define SYSTEM_UPDATE   3600000     //Send data system to broker (in miliseconds)
                                        //1 h = 60 min = 3600 s = 3600000 ms
#else
    #define SYSTEM_UPDATE   1000
#endif

#define DEBOUNCE        500

//Unique for each hardware
#define UUID        "83924a87-e7f6-4c75-95d7-dc9a8632ef45"  //uuid for hardware on Sensoriando


/* 
 * GlobalVariables
 */
long SystemElapsedTime;
byte InitializedSd, InitializedEth;
Nanoshield_RTC rtcclient;
EthernetClient ethernetclient;
SensoriandoObj sensoriando(ethernetclient);

enum LogMode{LM_Info, LM_Warning, LM_Error};
long SdcardSize;
uint8_t Mac[6];

/*
 * prototypes
 */
void(* resetFunc) (void) = 0; //declare reset function @ address 0      
void logthing(char *, int);

          
/*
 * Setting
 */
void setup()
{
    DateTime dt_rtc;
    
    #ifdef DEBUG
        Serial.begin(9600);
        Serial.println("Setting...");
    #endif

#ifdef DEBUG
Serial.print("Data struct (bytes): ");Serial.println(sizeof(SensoriandoSensorDatum));
Serial.print("Comand struct (bytes): ");Serial.println(sizeof(SensoriandoWifiCommandInit));
#endif


    /*
     * GPIO Setting
     */  
    interface_init();    
    interface_modeconfig();


    /* 
     * Inits assets OFFLINE
     */
    
    //Real Time Counter (RTC)    
    if ( ! rtc_init(&rtcclient) ) {
        interface_modeerror();
        logthing(RTC_INITFAIL, LM_Error);
        resetFunc();
    } else {
        logthing(RTC_INITPASS, LM_Info);       
    }

    //microSD
    SdcardSize = sd_fullsize();   
    InitializedSd = sd_init();

    if ( ! InitializedSd ){
        interface_modeerror();
        logthing(SD_INITFAIL, LM_Warning);
    } else {
        logthing(SD_INITPASS, LM_Info); 
        
    }
    
    //Ethernet
    InitializedEth = ethernet_init(Mac);

    if ( ! InitializedEth ){
        interface_modeerror();
        logthing(ETHERNET_DONOTCONFIG, LM_Warning);
    } else {
        logthing(ETHERNET_PASS, LM_Info);        
    }

    // Valid Erros
    if ( (! InitializedSd) && (! InitializedEth) ) {
        interface_modeerror();
        logthing(SYS_REBOOT, LM_Error);
        resetFunc();
    } else {
      interface_modeconfig();
    }


    /*
     * Inits ONLINE
     */
    // RTC Update  
    if ( ! rtc_check(&rtcclient) ) {
        interface_modeerror();
        logthing(RTC_UPDFAIL, LM_Error);
        resetFunc();
    } else {
        rtc_sync(&rtcclient, ntp_get(), dt_rtc);
        logthing(RTC_UPDPASS, LM_Info);
    }

    //Sensoriando   
    if ( !sensoriandoInit(&sensoriando, Mac) ) {
        interface_modeerror();
        logthing(BROKER_FAIL, LM_Warning);
    } else {
        logthing(BROKER_PASS, LM_Info);      
    }

    // Wifi    
    dt_rtc = rtc_get(&rtcclient); 

#ifdef DEBUG
Serial.print("Unix time: ");Serial.println(dt_rtc.unixtime());
#endif
    
    if ( !wifi_init(dt_rtc.unixtime()) ) {
        interface_modeerror();
        logthing(WIFI_FAIL, LM_Error);
        resetFunc();
    } else {
        logthing(WIFI_PASS, LM_Info);
    }


    /*
     * Done
     */
    interface_modenormal();
    logthing(WAIT_READ, LM_Info);
    
    SystemElapsedTime = millis();    
}
 
void loop()
{   
    SensoriandoSensorDatum datum = {NULL, NULL, NULL, NULL, NULL, NULL};
    SensoriandoParser sensoring;
    DateTime dt;
    long reset_elapsedtime;
    static long interface_elapsedtime = millis();


    /*
     * Pairing
     */
    if ( interface_pair() ) {
        delay(DEBOUNCE);
         
        logthing(SYS_PAIR, LM_Info);
        wifi_pair();       
    }

    
    /*
     * Check critical devices
     */
    if ( ! InitializedSd ) {
        if ( ! sd_init() ) {
            interface_elapsedtime = interface_modeerror();
            logthing(SD_INITFAIL, LM_Warning);
        } else {
            InitializedSd = 1;
        }
    }

    if ( ! InitializedEth ) {
        if ( ! ethernet_init(Mac) ) {
            interface_elapsedtime = interface_modeerror();
            logthing(ETHERNET_DONOTCONFIG, LM_Warning);
        } else {
            InitializedEth = 1;
        }
    }


    /*
     * Receive/Send Data
     */
    if ( (millis() - SystemElapsedTime) > SYSTEM_UPDATE ) {
        SystemElapsedTime = millis();
        dt = rtc_get(&rtcclient); 

        wifi_update(dt.unixtime());
        logthing(WIFI_UPD, LM_Info);
        
        if ( sensoriandoReconnect(&sensoriando, Mac) ) {
            interface_modesend(interface_elapsedtime);
            
            logthing(SYS_SEND_TIME, LM_Info);
            strcpy(sensoring.uuid, UUID);
            sensoring.dt = dt.unixtime();
            sensoriandoSendDatetime(&sensoriando, &sensoring);

            logthing(SYS_SEND_STORAGE, LM_Info);
            strcpy(sensoring.uuid, UUID);
            sensoring.dt = dt.unixtime();
            sensoring.value = sd_freespace(SdcardSize);
            sensoriandoSendStorage(&sensoriando, &sensoring); 
            
            logthing(SYS_SENT, LM_Info);
            interface_modenormal();
        } else {
            logthing(BROKER_CONN, LM_Warning);  
        }
    }

    if ( wifi_available(&datum) ) {
#ifdef DEBUG
Serial.print("[Send MQTT] Bytes received: ");Serial.println(sizeof(datum), DEC);
Serial.print("STX: ");Serial.println(datum.stx, HEX);
Serial.print("UUID: ");Serial.println(datum.uuid);
Serial.print("id: ");Serial.println(datum.id, DEC);
Serial.print("value: ");Serial.println(datum.value, DEC);
Serial.print("dt: ");Serial.println(datum.dt, DEC);
Serial.print("ETX: ");Serial.println(datum.etx, HEX);
Serial.println();
#endif          
        if ( sensoriandoReconnect(&sensoriando, Mac) ) {
            interface_modesend(interface_elapsedtime);

            strcpy(sensoring.uuid, datum.uuid);
            sensoring.id = datum.id;
            sensoring.dt = datum.dt;
            sensoring.value = datum.value;
            sensoriandoSendValue(&sensoriando, &sensoring); 
            
            interface_modenormal();
        } else {
            logthing(BROKER_SENSOR, LM_Warning);  

            interface_elapsedtime = interface_modeerror();
            sd_writedatum(&datum);
        }
    }

}


/*
 * functions
 */
void logthing(char *msg, int logmode)
{
    SensoriandoParser sensoring;
    char logmsg[256];
    DateTime dt;
    
    dt = rtc_get(&rtcclient);
    
    if ( !rtc_check(&rtcclient) ) {
        sprintf(logmsg, "[           system init           ] %s", msg);
    } else {     
        sprintf(logmsg, "[%02d/%02d/%04d %02d:%02d:%02d UTC] %s", dt.day(), dt.month(), dt.year(), \
                                                              dt.hour(), dt.minute(), dt.second(), \
                                                              msg);      
    }

    if ( logmode == LM_Info ) {
        #ifdef DEBUG
            Serial.println(msg);
        #endif  
    }

    if ( InitializedEth && (logmode == LM_Error) ) {
        if ( sensoriandoReconnect(&sensoriando, Mac) ) {
            strcpy(sensoring.uuid, UUID);
            sensoring.dt = dt.unixtime();
            strcpy(sensoring.msg, msg);
    
            sensoriandoSendMessage(&sensoriando, &sensoring);
        }
    }
        
    if ( InitializedSd && ((logmode == LM_Warning) || (logmode == LM_Error)) ) {
        sd_writemsg(logmsg);
    }
}
