/*
 * 
 */
#ifndef MESSAGE_H
#define MESSAGE_H
 
#define WAIT_READ           "Waiting..."
#define SYS_REBOOT          "Critical error: rebooting"
#define SYS_SENT            "Data system sent to broker"
 
#define WIFI_RESET          "Wifi setting reseted"
#define WIFI_FAIL           "Wifi fail: reboot!"
#define WIFI_PASS           "Wifi init, OK!"

#define RTC_INITFAIL        "RTC fail: reboot!"
#define RTC_INITPASS        "RTC init, OK!"
#define RTC_UPDFAIL         "RTC wasn't init: reboot!"
#define RTC_UPDPASS         "Update RTC via NTP, OK!"
#define RTC_NOW             "Unix time: %s"

#define MQTT_CRC            "CRC do not match"
#define MQTT_PASS           "MQTT init, OK!"
#define MQTT_FAIL           "MQTT passed!"
#define MQTT_SENSOR         "[Data Sensor] Broker don't connected: Writing SD!"

#define SD_INITFAIL             "SD fail: reboot!"
#define SD_INITPASS             "SD init, OK!"

#define ETHERNET_DONOTCONFIG      "Nao foi possivel configurar a placa de rede."
#define ETHERNET_NOTFOUND         "Placa de rede nao encontrada."
#define ETHERNET_CABLENOTCONNECT  "Cabo da placa de rede nao esta conectado."
#define ETHERNET_MYIP             "Meu endereco de IP: "
#define ETHERNET_PASS             "Ethernet init OK!"

#endif

 
