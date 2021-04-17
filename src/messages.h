/*
 * 
 */
#ifndef MESSAGE_H
#define MESSAGE_H
 
#define WAIT_READ           "Waiting..."

#define SYS_REBOOT          "Critical error: rebooting"
#define SYS_SENT            "Data system sent to broker"
#define SYS_PAIR            "Pairing started"
#define SYS_SEND_TIME       "Current date time send to broker"
#define SYS_SEND_STORAGE    "Current free space send to broker"

#define WIFI_FAIL           "Wifi fail: reboot!"
#define WIFI_PASS           "Wifi init, OK!"
#define WIFI_UPD            "Wifi updated with date and time"

#define RTC_INITFAIL        "RTC fail: reboot!"
#define RTC_INITPASS        "RTC init, OK!"
#define RTC_UPDFAIL         "RTC wasn't init: reboot!"
#define RTC_UPDPASS         "Update RTC via NTP, OK!"
#define RTC_NOW             "Unix time: %s"
#define RTC_READ            "Unix time updated from RTC"

#define BROKER_CRC            "CRC do not match"
#define BROKER_PASS           "Broker init, OK!"
#define BROKER_FAIL           "Broker initt, FAIL!"
#define BROKER_SENSOR         "Datum do not sent to Broker: Writing in SD!"
#define BROKER_TIME           "RTC do not sent to Broker"
#define BROKER_STORAGE        "Storage do not sent do Broker"
#define BROKER_CONN           "Broker do not connected"

#define SD_INITFAIL             "SD fail: reboot!"
#define SD_INITPASS             "SD init, OK!"

#define ETHERNET_DONOTCONFIG      "Nao foi possivel configurar a placa de rede."
#define ETHERNET_NOTFOUND         "Placa de rede nao encontrada."
#define ETHERNET_CABLENOTCONNECT  "Cabo da placa de rede nao esta conectado."
#define ETHERNET_MYIP             "Meu endereco de IP: "
#define ETHERNET_PASS             "Ethernet init OK!"

#endif

 
