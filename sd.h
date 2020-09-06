/*
 * 
 */
#ifndef SD_H
#define SD_H

#include <SPI.h>
#include <SD.h>

#ifndef GPIO_H
  #include "gpio.h"
#endif


/*
 * MACROS
 */
#define DEBUG_SD

#define MESSAGE_LOG   "logsys.txt"
#define DATA_DB       "data.hex"


/*
 * GlobalVariables
 */
//float SdFreeSpace;


/*    
 * Prototypes
 */
byte sd_init();
void sd_writemsg(char *);
void sd_writedatum(SensoriandoSensorDatum *);
float sd_fullsize();
float sd_freespace();
byte sd_readdatum(SensoriandoSensorDatum *);
void sd_dropdb();
 
/*
 * functions
 */
void sd_dropdb()
{
  File f;
  
  SD.remove(DATA_DB);
  
  f = SD.open(DATA_DB, FILE_WRITE);
  f.close();
}

byte sd_readdatum(SensoriandoSensorDatum *datum)
{
    File f;
    byte buf[sizeof(SensoriandoSensorDatum)];
    static unsigned long pos=0;

    datum->stx = NULL;
    datum->id = NULL;
    datum->value = NULL;
    datum->etx = NULL;
    
#ifdef DEBUG_SD
Serial.println("Reading DATA...");
#endif                

    f = SD.open(DATA_DB);
    f.seek(pos);
Serial.print(">> ");Serial.println(f.available());           
    if ( f.available() ) {
        f.read(buf, sizeof(buf));
        pos = f.position();      

        if ( f.available() ) {
            f.close();
        } else {
            f.close();
            sd_dropdb();
            pos = 0;            
        }
          
        memcpy(datum, buf, sizeof(buf));
    
#ifdef DEBUG_SD
Serial.print("STX: 0x0");Serial.println(datum->stx, HEX);
Serial.print("id: ");Serial.println(datum->id, DEC);
Serial.print("value: ");Serial.println(datum->value, DEC);
Serial.print("ETX: 0x0");Serial.println(datum->etx, HEX);
Serial.println();
#endif        
    } else {
#ifdef DEBUG_SD
Serial.println("DATA file empty");
#endif 
        pos = 0;      
    }
    
    return (datum->stx == STX) && (datum->etx == ETX);
}

byte sd_init() 
{ 
  byte res;
  File f;

  //Check card 
#ifdef DEBUG_SD
Serial.print("GPIO SD: ");Serial.println(GPIO_SD);
Serial.println("Initializing SD card...");
#endif

  res = SD.begin(GPIO_SD);    

  if ( res ) {           
      if ( ! SD.exists(MESSAGE_LOG) ) {
#ifdef DEBUG_SD
Serial.println("Don't exists LOG file");
#endif                
          f = SD.open(MESSAGE_LOG, FILE_WRITE);
          f.close();
      }       

      if ( ! SD.exists(DATA_DB) ) {
#ifdef DEBUG_SD
Serial.println("Don't exists DATUM file");
#endif                
          f = SD.open(DATA_DB, FILE_WRITE);
          f.close();
      } 

      //SdFreeSpace = sd_fullsize();    
  }
    
  return res;
}

void sd_writemsg(char *msg) 
{
    File f;
/*
    if ( ! SdFreeSpace ) {
#ifdef DEBUG_SD
Serial.println("[LOG] Out space!");
#endif                
        SD.remove(MESSAGE_LOG);      
    }
*/    
#ifdef DEBUG_SD
Serial.println("Writing MESSAGE log...");
#endif                
       
    f = SD.open(MESSAGE_LOG, FILE_WRITE);
    f.println(msg);
    f.close();     
}

void sd_writedatum(SensoriandoSensorDatum *datum) 
{
    File f;
/*    
    if ( ! SdFreeSpace ) {
#ifdef DEBUG_SD
Serial.println("[DATA] OUT SPACE");
#endif     
        SD.remove(DATA_DB);      
    }
*/
#ifdef DEBUG_SD
Serial.println("Writing DATA log...");
Serial.write((byte *)datum, sizeof(SensoriandoSensorDatum));
#endif     

    f = SD.open(DATA_DB, FILE_WRITE);
    f.write((byte *)datum, sizeof(SensoriandoSensorDatum));
    f.close();    
/*    
#ifdef DEBUG_SD
Serial.print("DATA size: ");Serial.print(f.size());    
#endif

    SdFreeSpace = SdFreeSpace - f.size()/1048576;
*/
}


float sd_freespace()
{
  return 0;//SdFreeSpace;
}

float sd_fullsize() 
{  
    uint32_t volumesize;
    float space;

    #ifdef ARDUINO
        SdVolume SdVolume;
        Sd2Card SdCard;
    #endif   

    #ifdef ARDUINO
        SdCard.init(SPI_HALF_SPEED, GPIO_SD);    
        SdVolume.init(SdCard);
        
        volumesize = SdVolume.blocksPerCluster();     // clusters are collections of blocks
        volumesize *= SdVolume.clusterCount();        // we'll have a lot of clusters
    #else //ESP8266
        volumesize = SD.blocksPerCluster();     // clusters are collections of blocks
        volumesize *= SD.clusterCount();        // we'll have a lot of clusters
    #endif
    
    volumesize /= 2;                              // SD card blocks are always 512 bytes (2 blocks are 1KB)
    space = volumesize / 1024.0;                   //Megabytes

#ifdef DEBUG_SD
Serial.print("Space MB: ");Serial.println(space);
#endif

    return space;  
}

#endif
