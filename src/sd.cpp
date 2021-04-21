#include "sd.h"

void movefile(char *source, char *destiny)
{
    File filein, fileout;

    filein = SD.open(source);
    fileout = SD.open(destiny, FILE_WRITE);
   
    if ( filein && fileout ) {
        while (filein.available() ) {
            fileout.write(filein.read());
        }
    }

    filein.close();
    fileout.close();

    SD.remove(source);
}


/*
 * Publics
 */
byte sd_readdatum(SensoriandoSensorDatum *datum)
{
    File root, entry;
    byte buf[sizeof(SensoriandoSensorDatum)];
    byte res=0;
    char filedata[32], filelost[32], filesent[32];

    datum->stx = NULL;
    datum->id = NULL;
    datum->value = NULL;
    datum->etx = NULL;
    
    root = SD.open(DATA_DIR);
    entry = root.openNextFile();
    
    if ( entry ) {
#ifdef DEBUG_SD
Serial.println("Reading DATUM in SD...");
#endif  
        res = 1;

        sprintf(filedata, "%s/%s", DATA_DIR, entry.name());
        sprintf(filelost, "%s/%s", LOSTFOUND_DIR, entry.name());
        sprintf(filesent, "%s/%s", SENT_DIR, entry.name());

        entry.read(buf, sizeof(buf));         
        memcpy(datum, buf, sizeof(buf));
    
#ifdef DEBUG_SD
Serial.print("STX: 0x0");Serial.println(datum->stx, HEX);
Serial.print("id: ");Serial.println(datum->id, DEC);
Serial.print("value: ");Serial.println(datum->value, DEC);
Serial.print("ETX: 0x0");Serial.println(datum->etx, HEX);
Serial.println();
#endif 

        if ( (datum->stx == STX) && (datum->etx == ETX) ) {
            movefile(filedata, filesent); 
        } else {
            movefile(filedata, filelost);    
        }
    }
    
    entry.close();
    root.close();

    return res;
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

      if ( ! SD.exists(DATA_DIR) ) {
#ifdef DEBUG_SD
Serial.println("Don't exists directory DATA");
#endif                
          SD.mkdir(DATA_DIR);
      } 

      if ( ! SD.exists(SENT_DIR) ) {
#ifdef DEBUG_SD
Serial.println("Don't exists directory SENT");
#endif                
          SD.mkdir(SENT_DIR);
      } 

      if ( ! SD.exists(LOSTFOUND_DIR) ) {
#ifdef DEBUG_SD
Serial.println("Don't exists directory LOST+FOUND");
#endif                
          SD.mkdir(LOSTFOUND_DIR);
      } 
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

#ifdef DEBUG_SD
Serial.println("Writed");
#endif
}

byte sd_writedatum(SensoriandoSensorDatum *datum) 
{
    File f;
    char filename[20];
    int res;

#ifdef DEBUG_SD
Serial.println("Writing DATUM...");
Serial.write((byte *)datum, sizeof(SensoriandoSensorDatum));
Serial.println();
#endif     

    sprintf(filename, "%s/%x%x.dat", DATA_DIR, datum->id, datum->dt);

#ifdef DEBUG_SD
Serial.print("Filename: ");Serial.println(filename);
#endif     

    f = SD.open(filename, FILE_WRITE);
    res = f.write((byte *)datum, sizeof(SensoriandoSensorDatum));
    f.close();

    return res;
}

long sd_freespace(long sdsize)
{
    File root;
    long used;

    root = SD.open("/");
    used = sd_usedsize(root, 0);

#ifdef DEBUG_SD
Serial.print("FULL Space in Kbytes: ");Serial.println(sdsize);
Serial.print("USED Space in Kbytes: ");Serial.println(used); 
Serial.print("FREE Space in Kbytes: ");Serial.println(sdsize-used);
#endif

    root.close();
    return sdsize-used;
}

long sd_fullsize() 
{  
    uint32_t volumesize;

    #ifdef ARDUINO
        SdVolume SdVolume;
        Sd2Card SdCard;
    #endif   

    #ifdef ARDUINO
        if ( ! SdCard.init(SPI_HALF_SPEED, GPIO_SD) ) {    
#ifdef DEBUG_SD
Serial.println("[SD] Card init fail");
#endif        
        } else {
#ifdef DEBUG_SD
Serial.println("[SD] Card wiring is correct and a card is present.");
#endif
        }

        if ( ! SdVolume.init(SdCard) ) {
#ifdef DEBUG_SD
Serial.println("[SD] Volume init fail");
#endif        
        } else {
#ifdef DEBUG_SD
Serial.println("[SD] Volume wiring is correct and a card is present.");
#endif
        }

        
        volumesize = SdVolume.blocksPerCluster();     // clusters are collections of blocks
        volumesize *= SdVolume.clusterCount();        // we'll have a lot of clusters
    #else //ESP8266
        volumesize = SD.blocksPerCluster();     // clusters are collections of blocks
        volumesize *= SD.clusterCount();        // we'll have a lot of clusters
    #endif
    
    volumesize /= 2;                              // SD card blocks are always 512 bytes (2 blocks are 1KB)
//       space = volumesize / 1024.0;                   //Megabytes

#ifdef DEBUG_SD
Serial.print("FULLSpace in Kbytes: ");Serial.println((long)volumesize);
#endif
  
    return (long)volumesize;  
}

long sd_usedsize(File dir, long bytes)
{
    File entry;

    while (true) {
        entry =  dir.openNextFile();
        
        if (! entry) {
            dir.rewindDirectory();
            break;
        } 

#ifdef DEBUG_SD
//if ( entry ) { Serial.println(entry.name()); };
#endif

        if (entry.isDirectory()) {
            bytes = sd_usedsize(entry, bytes);
        } else {
            bytes = bytes + entry.size();
        }

        entry.close();
    }

    return bytes/1024;
}


