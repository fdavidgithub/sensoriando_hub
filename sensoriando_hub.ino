/* 
 * Read sound sensor and publish in MQTT server
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
#include "src/mqtt.h"
#include "src/led.h"
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

#define THING_RESET     5000
#define THING_DEBOUNCE  1000


/* 
 * GlobalVariables
 */
long system_elapsedtime;


/*
 * prototypes
 */
void(* resetFunc) (void) = 0; //declare reset function @ address 0      
void logthing(char *);

          
/*
 * Setting
 */
void setup()
{
    char logmsg[256];
    DateTime dt_rtc;
    byte status_sd, status_ethernet;

    #ifdef DEBUG
        Serial.begin(9600);
        Serial.println("Setting...");
    #endif


    /*
     * GPIO Setting
     */
    #ifdef GPIO_RESET
      pinMode(GPIO_RESET, INPUT);

#ifdef DEBUG
Serial.printf("GPIO RESET %d\n", GPIO_RESET);
#endif
    #endif

    led_init();    
    led_modeconfig();


    /* 
     * Inits OFFLINE
     */
    //Real Time Counter (RTC)    
    if ( ! rtc_init() ) {
        led_modeerror();
        logthing(RTC_INITFAIL);
        resetFunc();
    } else {
        logthing(RTC_INITPASS);       
    }

    //microSD
    status_sd = sd_init();

    if ( ! status_sd ){
        led_modeerror();
        logthing(SD_INITFAIL);
    } else {
        logthing(SD_INITPASS); 
    }
    
    //Ethernet
    status_ethernet = ethernet_init();

    if ( ! status_ethernet ){
        led_modeerror();
        logthing(ETHERNET_DONOTCONFIG);
/*
        if (Ethernet.hardwareStatus() == EthernetNoHardware) {
            logthing(ETHERNET_NOTFOUND);
        } else if (Ethernet.linkStatus() == LinkOFF) {
            logthing(ETHERNET_CABLENOTCONNECT);
        }
      
        resetFunc();
*/
    } else {
        logthing(ETHERNET_PASS);        
    }

    // Valid Erros
    if ( (! status_sd) && (! status_ethernet) ) {
        led_modeerror();
        logthing(SYS_REBOOT);
        resetFunc();
    }


    /*
     * Inits ONLINE
     */
    // RTC Update  
    if ( ! rtc_check() ) {
        led_modeerror();
        logthing(RTC_UPDFAIL);
        resetFunc();
    } else {
        rtc_sync(ntp_get(), dt_rtc);
        logthing(RTC_UPDPASS);
    }

    // MQTT
    if ( !mqtt_init() ) {
        led_modeerror();
        logthing(MQTT_FAIL);
    } else {
        logthing(MQTT_PASS);      
    }
       
    // Wifi    
    dt_rtc = rtc_get(); 

#ifdef DEBUG
Serial.print("Unix time: ");Serial.println(dt_rtc.unixtime());
#endif
    
    if ( !wifi_init(dt_rtc.unixtime()) ) {
        led_modeerror();
        logthing(WIFI_FAIL);
        resetFunc();
    } else {
        logthing(WIFI_PASS);
    }
 

    /*
     * Done
     */
    led_modenormal();
    logthing(WAIT_READ);
    
    system_elapsedtime = millis();    
}
 
void loop()
{   
    SensoriandoSensorDatum datum = {NULL, NULL, NULL, NULL, NULL};
    DateTime dt;
    long reset_elapsedtime;


    /*
     * Check if is necessary reset system
     */
    #ifdef GPIO_RESET
    if ( digitalRead(GPIO_RESET) ) {
        led_modeerror();
        reset_elapsedtime=millis();
        
        while ( ((millis() - reset_elapsedtime) < THING_RESET) && digitalRead(GPIO_RESET) ) {
#ifdef DEBUG
Serial.println(reset_elapsedtime);
#endif
        }

        if ( (millis()-reset_elapsedtime) >= THING_RESET && digitalRead(GPIO_RESET) ) {
            logthing(WIFI_RESET);    
            delay(THING_DEBOUNCE);
            resetFunc();
        } else {
            led_modenormal();
        }
    }
    #endif


    /*
     * Receive/Send Data
     */
    if ( (millis() - system_elapsedtime) >= SYSTEM_UPDATE ) {
        system_elapsedtime = millis();
        dt = rtc_get(); 

        if ( mqtt_reconnect() ) {
            led_modesend();

            logthing(SYS_SENT);   
            mqtt_senddatetime(dt, dt.unixtime());
            mqtt_sendstorage(dt, sd_freespace()); 

            led_modenormal();
        }
    }

    if ( wifi_available(&datum) ) {
#ifdef DEBUG
Serial.print("[Send MQTT] Bytes received: ");Serial.println(sizeof(datum), DEC);
Serial.print("STX: ");Serial.println(datum.stx, HEX);
Serial.print("id: ");Serial.println(datum.id, DEC);
Serial.print("value: ");Serial.println(datum.value, DEC);
Serial.print("dt: ");Serial.println(datum.dt, DEC);
Serial.print("ETX: ");Serial.println(datum.etx, HEX);
Serial.println();
#endif          
        if ( mqtt_reconnect() ) {
            led_modesend();
            mqtt_sendvalue(datum.dt, datum.value, datum.id); 
            led_modenormal();
        } else {
            logthing(MQTT_SENSOR);
            sd_writedatum(&datum);
        }
    }

}


/*
 * functions
 */
void logthing(char *msg)
{
    char logmsg[256];
    DateTime dt;
    
#ifdef DEBUG
Serial.println(msg);delay(1000);
#endif

    dt = rtc_get();
    
    if ( !rtc_check() ) {
      sprintf(logmsg, "[           system init           ] %s", msg);
    } else {     
      sprintf(logmsg, "[%02d/%02d/%04d %02d:%02d:%02d UTC] %s", dt.day(), dt.month(), dt.year(), \
                                                              dt.hour(), dt.minute(), dt.second(), \
                                                              msg);      
    }

    mqtt_sendmessage(dt, msg);
    sd_writemsg(logmsg);
}
