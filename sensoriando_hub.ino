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

#include "gpio.h"
#include "messages.h"
#include "sd.h"
#include "rtc.h"
#include "ntp.h"
#include "mqtt.h"
#include "led.h"
#include "ethernet.h"
#include "wifi.h"


/*
 * MACROS
 */
#define DEBUG

#define THING_UPDATE    1000    //Update server in miliseconds
#define THING_RESET     5000
#define THING_DEBOUNCE  1000


/* 
 * GlobalVariables
 */
long update_elapsedtime;


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
    if ( ! sd_init() ){
        led_modeerror();
        logthing(SD_INITFAIL);
        resetFunc();
    } else {
        logthing(SD_INITPASS); 
    }
    
    //Ethernet
    if ( ! ethernet_init() ){
        led_modeerror();
        logthing(ETHERNET_DONOTCONFIG);
/*
        if (Ethernet.hardwareStatus() == EthernetNoHardware) {
            logthing(ETHERNET_NOTFOUND);
        } else if (Ethernet.linkStatus() == LinkOFF) {
            logthing(ETHERNET_CABLENOTCONNECT);
        }
*/      
        resetFunc();
    } else {
        logthing(ETHERNET_PASS);        
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
        resetFunc();      
    } else {
        logthing(MQTT_PASS);      
    }
       
    // Wifi    
    if ( !wifi_init() ) {
        led_modeerror();
        logthing(WIFI_FAIL);
        resetFunc();
    } else {
        logthing(WIFI_PASS);
    }
 
    // Done
    led_modenormal();
    update_elapsedtime = millis();

    logthing(WAIT_READ);
}
 
void loop()
{   
    SensoriandoSensorDatum datum = {NULL, NULL, NULL, NULL};
    char sent[256];
    DateTime dt;
    byte i;
    long reset_elapsedtime=0;


    /*
     * Check if is necessary new ESSID
     */
    #ifdef GPIO_RESET
    if ( digitalRead(GPIO_RESET) ) {
        led_modeerror();
        
        while ( reset_elapsedtime < THING_RESET && digitalRead(GPIO_RESET) ) {
            reset_elapsedtime++;
            delay(1);

#ifdef DEBUG
Serial.println(reset_elapsedtime);
#endif
        }

        if ( reset_elapsedtime >= THING_RESET && digitalRead(GPIO_RESET) ) {
            logthing(WIFI_RESET);
            
            delay(THING_DEBOUNCE);
            resetFunc();
        } else {
            led_modenormal();
        }
    }
    #endif


    /*
     * Receive data
     */
    if ( wifi_available(&datum) ) {
        sd_writedatum(&datum);  
    }

    if ( (millis() - update_elapsedtime) >= THING_UPDATE ) {
        update_elapsedtime = millis();

        mqtt_reconnect();

        dt = rtc_get(); 

        led_modesend();
        
        mqtt_senddatetime(dt, dt.unixtime());
        mqtt_sendstorage(dt, sd_freespace()); 

        while ( sd_readdatum(&datum) ) {
          mqtt_sendvalue(dt, datum.value, datum.id);  
        }

        led_modenormal();
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
