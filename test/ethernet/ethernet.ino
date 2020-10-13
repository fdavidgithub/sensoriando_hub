#include <SPI.h>
#include <Ethernet.h>
#include "gpio.h"

//#include <Wire.h>
//#include <Nanoshield_Ethernet.h>
#include <Nanoshield_EEPROM.h>

// The MAC address will be read from the EEPROM on the Ethernet Nanoshield
byte mac[6];

// Set the static IP address to use if the DHCP fails to assign
//IPAddress ip(10, 0, 0, 112);

// Inialize EEPROM with MAC address
Nanoshield_EEPROM eeprom(1, 1, 0, true);

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "www.google.com";    // name address for Google (using DNS)

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);


  // Read the MAC address from the Ethernet Nanoshield EEPROM and show it on the terminal
  eeprom.begin();
  eeprom.startReading(0x00FA);

  Serial.println();
  Serial.print("MAC address: ");
  
  for (int i = 0; i < 5; i++) {
    mac[i] = eeprom.read();
    Serial.print(mac[i], HEX);
    Serial.print(":");
  }
  
  mac[5] = eeprom.read();
  Serial.println(mac[5], HEX);

  // initialize the ethernet device
  if ( !Ethernet.begin(mac) ) {
      Serial.println("Failed to configure Ethernet using DHCP"); 
      // no point in carrying on, so do nothing forevermore:
      // try to congifure using IP address instead of DHCP:
      //Ethernet.begin(mac, ip);
  } else {
      Serial.println("Configured!");
  }

  // give the Ethernet shield a second to initialize:
  delay(1000);

  // print your local IP address:
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");      
  }
  

  Serial.println();
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println("GET /search?q=arduino HTTP/1.1");
    client.println("Host: www.google.com");
    client.println("Connection: close");
    client.println();
  }
  else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
  }

}

void loop()
{

}
