/*
  SD card test

 This example shows how use the utility libraries on which the'
 SD library is based in order to get info about your SD card.
 Very useful for testing a card when you're not sure whether its working or not.

 The circuit:
  * SD card attached to SPI bus as follows:
 ** MOSI - pin 11 on Arduino Uno/Duemilanove/Diecimila
 ** MISO - pin 12 on Arduino Uno/Duemilanove/Diecimila
 ** CLK - pin 13 on Arduino Uno/Duemilanove/Diecimila
 ** CS - depends on your SD card shield or module.
 		Pin 4 used here for consistency with other Arduino examples


 created  28 Mar 2011
 by Limor Fried
 modified 9 Apr 2012
 by Tom Igoe
 */
 // include the SD library:
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <MultiLCD.h>

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

LCD_ILI9341 lcd;

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
const int chipSelect = 10;

void setup()
{
  lcd.begin();
  lcd.setFont(FONT_SIZE_SMALL);
  lcd.setTextColor(RGB16_YELLOW);
  lcd.println("Initializing SD card...");

  pinMode(chipSelect, OUTPUT);

  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_FULL_SPEED, chipSelect)) {
    lcd.setTextColor(RGB16_RED);
    lcd.println("\nInitialization failed. Things to check:");
    lcd.setTextColor(RGB16_WHITE);
    lcd.println("* Is a card is inserted?");
    lcd.println("* Is your wiring correct?");
    lcd.println("* Does the chipSelect pin match your shield?");
    return;
  } else {
    lcd.setTextColor(RGB16_CYAN);
    lcd.println("Wiring is correct and a card is present.");
  }

  // print the type of card
  lcd.setTextColor(RGB16_GREEN);
  lcd.print("\nCard Type: ");
  switch(card.type()) {
    case SD_CARD_TYPE_SD1:
      lcd.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      lcd.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      lcd.println("SDHC");
      break;
    default:
      lcd.println("Unknown");
  }

  lcd.setTextColor(RGB16_WHITE);
  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    lcd.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }


  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  lcd.print("\nVolume Type: FAT");
  lcd.println(volume.fatType(), DEC);
  lcd.println();

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                            // SD card blocks are always 512 bytes
  lcd.print("Volume size: ");
  lcd.print(volumesize);
  lcd.print(" bytes (");
  lcd.print(volumesize >> 20);
  lcd.println("MB)");

  lcd.println("\nFiles found on SD (name, date and size in bytes)");
  root.openRoot(volume);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
}


void loop(void) {

}
