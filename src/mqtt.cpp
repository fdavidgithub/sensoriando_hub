#include "mqtt.h"

byte mqtt_init(PubSubClient *mqttclient)
{
    byte res;
    
    mqttclient->setServer(MQTT_SERVER, MQTT_PORT);  
    res = mqttclient->connect("Sensoriando", MQTT_USER, MQTT_PASSWD);
    return res;
}
 
byte mqtt_reconnect(PubSubClient *mqttclient) 
{
    if ( !mqttclient->connected() ) {
#ifdef DEBUG_MQTT
        Serial.print("Attempting MQTT connection...");
#endif
        if ( mqttclient->connect("Sensoriando", MQTT_USER, MQTT_PASSWD)) {
#ifdef DEBUG_MQTT
            Serial.println("MQTT Server Connected");
#endif

        } else {
#ifdef DEBUG_MQTT
            Serial.print("failed, rc=");
            Serial.print(mqttclient->state());
#endif      
        }
    }

    return mqttclient->connected();
}


void mqtt_sendvalue(PubSubClient *mqttclient, SensoriandoParser *sensoring) 
{
    char payload[MQTT_LEN], topic[MQTT_LEN];

    sensoriandoSendValue(sensoring, payload, topic);
   
    mqttclient->publish(topic, payload);    
}

void mqtt_senddatetime(PubSubClient *mqttclient, SensoriandoParser *sensoring) 
{
    char payload[MQTT_LEN], topic[MQTT_LEN];

    sensoriandoSendDatetime(sensoring, payload, topic);
    
    mqttclient->publish(topic, payload);
}

void mqtt_sendstorage(PubSubClient *mqttclient, SensoriandoParser *sensoring) 
{
    char payload[MQTT_LEN], topic[MQTT_LEN];

    sensoriandoSendStorage(sensoring, payload, topic);

    mqttclient->publish(topic, payload);
}

void mqtt_sendmessage(PubSubClient *mqttclient, SensoriandoParser *sensoring) 
{
    char payload[MQTT_LEN], topic[MQTT_LEN];

    sensoriandoSendMessage(sensoring, payload, topic);

    mqttclient->publish(topic, payload);
}


