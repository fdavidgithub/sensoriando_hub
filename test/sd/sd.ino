#include <SPI.h>
#include <SD.h>
#include "gpio.h"

void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      dir.rewindDirectory();
      break;
    }

    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }

    Serial.print(entry.name());
    
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

float used(File dir, float w) 
{
  float bytes=w;
  File entry;

  while (true) {
    entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      dir.rewindDirectory();
      break;
    }

    if (entry.isDirectory()) {
        used(entry, bytes);
    }

    bytes = bytes + entry.size();
    entry.close();
 }

    return bytes;
}

float space()
{
    uint32_t volumesize;
    float space;

    SdVolume SdVolume;
    Sd2Card SdCard;

        SdCard.init(SPI_HALF_SPEED, GPIO_SD);    
        SdVolume.init(SdCard);
        
        volumesize = SdVolume.blocksPerCluster();     // clusters are collections of blocks
        volumesize *= SdVolume.clusterCount();        // we'll have a lot of clusters
    
    volumesize /= 2.0;                              // SD card blocks are always 512 bytes (2 blocks are 1KB)
    space = volumesize / 1024.0;                    //Mbytes

    return space;  
}

void setup() {
  // Open serial communications and wait for port to open:
  int init=0;
  File myFile;
  
  Serial.begin(9600);

  Serial.println();
  Serial.println("Initializing SD card...");
  Serial.print("GPIO_SD ");Serial.println(GPIO_SD);
  
  init = SD.begin(GPIO_SD);
  
  if (!init) {
    Serial.println("initialization failed!");
  } else {
    Serial.println("initialization done.");
  }

  if ( init ) { 
    if (SD.exists("example.txt")) {
      Serial.println("example.txt exists.");
    } else {
      Serial.println("example.txt doesn't exist.");
      
      // open a new file and immediately close it:
      Serial.println("Creating example.txt...");
      myFile = SD.open("example.txt", FILE_WRITE);
      myFile.close();
    }
   
    // Check to see if the file exists:
    if (SD.exists("example.txt")) {
      Serial.println("example.txt exists.");

      // delete the file:
      Serial.println("Removing example.txt...");
      SD.remove("example.txt");
  
//      myFile = SD.open("/");
//      printDirectory(myFile, 0); 
    } else {
      Serial.println("example.txt doesn't exist.");
    }  
  }
  
}

void loop() {
  File myFile;

  myFile = SD.open("example.txt", FILE_WRITE);
  if (myFile) {
    myFile.println("teste");
  } else {
    Serial.println("[example.txt]do not open file");
  }
  myFile.close();

  delay(1000);

  myFile = SD.open("/");
  
  printDirectory(myFile, 0);  
  
  Serial.println(space());
  Serial.println(used(myFile,0));
}
