/*
 * 
 */
#ifndef MQTT_H
#define MQTT_H

#include <Arduino.h>
#include <RTClib.h>
#include <PubSubClient.h>
#include <sensoriando.h>

#ifndef ETHERNET_H
  #include "ethernet.h"
#endif


/*
 * MACROS
 */
//#define DEBUG_MQTT

#define MQTT_SERVER "broker.sensoriando.com.br"
#define MQTT_PORT   1883
#define MQTT_USER   "fdavid"
#define MQTT_PASSWD "12345678"
#define MQTT_LEN    256

/*
 * Prototypes
 */
byte mqtt_init(PubSubClient *);
byte mqtt_reconnect(PubSubClient *);
void mqtt_sendvalue(PubSubClient *, SensoriandoParser *);
void mqtt_senddatetime(PubSubClient *, SensoriandoParser *);
void mqtt_sendstorage(PubSubClient *, SensoriandoParser *);
void mqtt_sendmessage(PubSubClient *, SensoriandoParser *);

#endif

