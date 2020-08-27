/*
 * 
 */
#ifndef ETHERNET_H
#define ETHERNET_H

#include <SPI.h>
#include <Wire.h>
#include <Nanoshield_Ethernet.h>
#include <Nanoshield_EEPROM.h>

/*
 * MACROS
 */
//#define DEBUG

/* 
 * GlobalVariables
 */
EthernetClient ethernetclient;


/*
 * Prototypes
 */
byte ethernet_init();

 
/*
 * functions
 */
byte ethernet_init() {
    byte mac[6];
    Nanoshield_EEPROM eeprom(1, 1, 0, true);
    byte i;
    byte res;
    
    // Read the MAC address from the Ethernet Nanoshield EEPROM and show it on the terminal
    eeprom.begin();
    eeprom.startReading(0x00FA);

    for (i = 0; i < 5; i++) {
      mac[i] = eeprom.read();
    }

#ifdef DEBUG    
Serial.println();Serial.print("MAC address: ");
  
for (i = 0; i < 5; i++) {
  Serial.print(mac[i], HEX);Serial.print(":");
}
#endif

    res = Ethernet.begin(mac);
    
#ifdef DEBUG
Serial.println("My IP address: ");Serial.print(Ethernet.localIP());
#endif

    return res;
}

#endif
