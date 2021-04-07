#include "ethernet.h"

byte ethernet_init(uint8_t *mac) {
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

if (Ethernet.hardwareStatus() == EthernetNoHardware) {
Serial.println("Ethernet not found");
} else if (Ethernet.linkStatus() == LinkOFF) {
Serial.println("Cable not connected");
}
#endif

    res = Ethernet.begin(mac);
    
#ifdef DEBUG_ETH
Serial.println("My IP address: ");Serial.print(Ethernet.localIP());
#endif

    return res;
}

