#include <SPI.h>
#include <SD.h>
#include <MultiLCD.h>

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

const int chipSelect = 10;

// set up access to LCD screen
LCD_ILI9341 lcd;

void setup()
{
  lcd.begin();
  lcd.setFontSize(FONT_SIZE_SMALL);
  lcd.setColor(RGB16_YELLOW);
  lcd.println("Initializing SD card...");

  pinMode(chipSelect, OUTPUT);

  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_FULL_SPEED, chipSelect)) {
    lcd.setColor(RGB16_RED);
    lcd.println("\nInitialization failed. Things to check:");
    lcd.setColor(RGB16_WHITE);
    lcd.println("* Is a card is inserted?");
    lcd.println("* Is your wiring correct?");
    lcd.println("* Does the chipSelect pin match your shield?");
    return;
  } else {
    lcd.setColor(RGB16_CYAN);
    lcd.println("Wiring is correct and a card is present.");
  }

  // print the type of card
  lcd.setColor(RGB16_GREEN);
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

  lcd.setColor(RGB16_WHITE);
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


void loop(void)
{
}
