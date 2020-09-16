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


void mqtt_sendvalue(PubSubClient *mqttclient, DateTime dt, float value_sensor, int id_sensor) {
    char msg[256], topic[256];
    
    sensoriandoSendValue(dt, value_sensor, id_sensor, msg, topic);
/*
    sprintf(msg, "{\"dt\":\"%04d%02d%02d%02d%02d%02d\", \"value\":%f}", \
                    dt.year(), dt.month(), dt.day(), \
                    dt.hour(), dt.minute(), dt.second(), \
                    value_sensor);   
    sprintf(topic, "%s/%d", MQTT_UUID, id_sensor);
*/    
    mqttclient->publish(topic, msg);
    
}

void mqtt_senddatetime(PubSubClient *mqttclient, DateTime dt, long value_dt) {
    char msg[256], topic[256];

    sensoriandoSendDatetime(dt, value_dt, msg, topic);
/*    
    sprintf(msg, "{\"dt\":\"%04d%02d%02d%02d%02d%02d\", \"value\":%d}", \
                  dt.year(), dt.month(), dt.day(), \
                  dt.hour(), dt.minute(), dt.second(), \
                  value_dt);   
    sprintf(topic, "%s/%d", MQTT_UUID, TIME_ID);
*/
    mqttclient->publish(topic, msg);
}

void mqtt_sendstorage(PubSubClient *mqttclient, DateTime dt, float value) {
    char msg[256], topic[256];

    sensoriandoSendStorage(dt, value, msg, topic);
/*    
    sprintf(msg, "{\"dt\":\"%04d%02d%02d%02d%02d%02d\", \"value\":%d}", \
                  dt.year(), dt.month(), dt.day(), \
                  dt.hour(), dt.minute(), dt.second(), \
                  value_dt);   
    sprintf(topic, "%s/%d", MQTT_UUID, TIME_ID);
*/
    mqttclient->publish(topic, msg);
}

void mqtt_sendmessage(PubSubClient *mqttclient, DateTime dt, char *msg) 
{
    char topic_msg[256], topic[256];

    sensoriandoSendMessage(dt, msg, topic_msg, topic);
/*    
    sprintf(topic_msg, "{\"dt\":\"%04d%02d%02d%02d%02d%02d\", \"value\":\"%s\"}", \
                    dt.year(), dt.month(), dt.day(), \
                    dt.hour(), dt.minute(), dt.second(), \
                    msg);   
    sprintf(topic, "%s/%d", MQTT_UUID, MESSAGE_ID);
*/
#ifdef DEBUG_MQTT
Serial.print("json: ");Serial.println(topic_msg);
#endif

    mqttclient->publish(topic, topic_msg);
}


