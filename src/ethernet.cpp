#include "ethernet.h"

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

#ifdef DEBUG_ETH    
Serial.println();Serial.print("MAC address: ");
  
for (i = 0; i < 5; i++) {
  Serial.print(mac[i], HEX);Serial.print(":");
}
#endif

    res = Ethernet.begin(mac);
    
#ifdef DEBUG_ETH
Serial.println("My IP address: ");Serial.print(Ethernet.localIP());
#endif

    return res;
}

