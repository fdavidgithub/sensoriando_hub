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
#define DEBUG

#define MESSAGE_LOG   "logsys.txt"
#define DATA_LOG      "logdata.txt"


/*
 * GlobalVariables
 */
float SdFreeSpace;


/*    
 * Prototypes
 */
byte sd_init();
void sd_writemsg(char *);
void sd_writedatum(char *);
float sd_fullsize();
float sd_freespace();

 
/*
 * functions
 */
byte sd_init() 
{ 
  byte res;
  File f;

  //Check card 
#ifdef DEBUG
Serial.print("GPIO SD: ");Serial.println(GPIO_SD);
Serial.println("Initializing SD card...");
#endif

  res = SD.begin(GPIO_SD);    

  if ( res ) {           
      if ( ! SD.exists(MESSAGE_LOG) ) {
#ifdef DEBUG
Serial.println("Don't exists LOG file");
#endif                
          f = SD.open(MESSAGE_LOG, FILE_WRITE);
          f.close();
      }       

      if ( ! SD.exists(DATA_LOG) ) {
#ifdef DEBUG
Serial.println("Don't exists DATUM file");
#endif                
          f = SD.open(DATA_LOG, FILE_WRITE);
          f.close();
      } 

      SdFreeSpace = sd_fullsize();    
  }
    
  return res;
}

void sd_writemsg(char *msg) 
{
    File f;

    if ( ! SdFreeSpace ) {
#ifdef DEBUG
Serial.println("[LOG] Out space!");
#endif                
        SD.remove(MESSAGE_LOG);      
    }
    
#ifdef DEBUG
Serial.println("Writing MESSAGE log...");
#endif                
       
    f = SD.open(MESSAGE_LOG, FILE_WRITE);
    f.println(msg);
    SdFreeSpace = SdFreeSpace - f.size()/1048576;
    f.close();     
}

void sd_writedatum(char *msg) 
{
    File f;

    if ( ! SdFreeSpace ) {
#ifdef DEBUG
Serial.println("[DATA] Out space!");
#endif                
        SD.remove(DATA_LOG);      
    }

#ifdef DEBUG
Serial.println("Writing DATA log...");
#endif     
      
    f = SD.open(DATA_LOG, FILE_WRITE);
    f.println(msg);

#ifdef DEBUG
Serial.print("DATA size: ");Serial.print(f.size());    
#endif

    SdFreeSpace = SdFreeSpace - f.size()/1048576;
    f.close();    
}


float sd_freespace()
{
  return SdFreeSpace;
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

#ifdef DEBUG
Serial.print("Space MB: ");Serial.println(space);
#endif

    return space;  
}

#endif
