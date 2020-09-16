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
#define MQTT_UUID   "27c36465-7356-4fe7-b726-cf619a9a22f0"
#define MQTT_PORT   1883
#define MQTT_USER   "fdavid"
#define MQTT_PASSWD "12345678"


/*
 * Prototypes
 */
byte mqtt_init(PubSubClient *);
byte mqtt_reconnect(PubSubClient *);
void mqtt_sendvalue(PubSubClient *, DateTime, float, int);
void mqtt_senddatetime(PubSubClient *, DateTime, long);
void mqtt_sendstorage(PubSubClient *, DateTime, float);
void mqtt_sendmessage(PubSubClient *, DateTime, char *);

#endif

