/*
 * GPIO Map
 */
#ifndef GPIO_H
#define GPIO_H

//#define GPIO_RESET   D0   
#define GPIO_RED     A2           
#define GPIO_GREEN   A1      
#define GPIO_BLUE    A0      

/*
 * Nanoshield microSD
 * MicroSD  BaseBoard ESP8266   Fucntion
 * /CS      D4        GPIO_SD   Chip select SPI
 * SDI      D11       D7        Bus SPI (MOSI)
 * SDO      D12       D6        Bus SPI (MISO)
 * SCK      D13       D5        Bus SPI (clock)
 * 3V3      3V3       --        Power supply 3,3V (opcional)
 * VCC      VCC       --        Power suppy 4.5~5.5V
 * VIN      VIN       --        External power supply 7~12v
 * GND      GND       --        Ground
 */
#ifdef ARDUINO
  #define GPIO_SD       4  
#else //ESP8266
  #define GPIO_SD       D4
#endif
  
/*
 * Nanoshield Ethernet
 * Ethernet BaseBoard ESP8266       Function
 * /INT     D2        --            Output interruptr W5500 (opcional)
 * /CS      D10#      GPIO_ETHERNET Chip Select W5500
 * SDI      D11#      D7            Bus SPI (MOSI)
 * SDO      D12       D6            Bus SPI (MISO)
 * SCLK     D13       D5            Bus SPI (clock)
 * SDA      A4        GPIO_SDA      I2C bus datak line
 * SCL      A5        GPIO_SCL      I2C bus clock line
 * VCC      VCC       --            Power suppy 4.5~5.5V
 * VIN      VIN       --            External power supply 7~12v
 * GND      GND       --            Ground
 */
#define GPIO_ETHERNET 10
  
/*
 * For the I²C bus
 * SDA (Optional, Pin): Data line, default values (usually GPIO21 for ESP32 and GPIO4 for ESP8266).
 * SCL (Optional, Pin): Clock line, defaultvalues (usually GPIO22 for ESP32 and GPIO5 for ESP8266).
 */
#define GPIO_SDA     SDA
#define GPIO_SCL     SCL

#endif
