MultiLCD Library for Arduino
============================

_Written by Stanley Huang, distributed under GPL._

MultiLCD is an Arduino library designed for displaying characters and bitmaps on different models of Arduino LCD display shields/modules with easy-to-use and unified API. It is developed with performance in the first priority. For supporting a wide range of devices and providing additional rendering APIs, the new implementation inherits a modified version of UTFT library. That means most APIs of UTFT are accessible via MultiLCD library.

The library embeds 2 types of font for ASCII characters (5x7, 8x16) and 4 types of fonts for digits (8x8, 8x16, 16x16, 16x24). It is extremely simple for display texts and numbers on desired position on a LCD screen with the library, while very little change in code is needed to switch from one LCD module to another. For more sophisticated drawing and rendering features, UTFT APIs can be used.

MultiLCD supports several color TFT LCD display shields/modules including:

    Freematics 3.2" TFT LCD shield (320x240@16bpp)
    Freematics 2.2" TFT LCD shield (320x240@16bpp)
    IteadStudio 2.8" TFT LCD shield (320x240@16bpp)
    All SSD1289 based TFT LCD
    All ILI9341 based TFT LCD (SPI)
    All ILI9325D based TFT LCD

MultiLCD has dumped support for monochrome LCD and OLED display since a while ago since starting to inherit UTFT library APIs. The support has been moved to MicroLCD library which shares the same APIs except for the UTFT APIs. It is also more optimized for Arduino with smaller program memory (Arduino Micro, Arduino Nano, Arduino Mini etc.). MicroLCD supports following monochrome display shields/modules:

    DFRobot LCD4884 shield (84x48)
    IteadStudio 0.96" OLED module (128x64)
    Freematics 1.3" OLED module (132x64)
    All Nokia 3310/5100 compatible LCD module (84x48)
    All SSD1306 based OLED module (128x64)
    All SH1106 based OLED module (132x64)

Usage
-----

You need to include library header at the beginning of the sketch.

```C++
#include <MultiLCD.h>
```

And use one of following declarations before your code:

Model                   | Code
----------------------- | ------------------------
```LCD_SSD1289 lcd;```  | SSD1289 based TFT LCD
```LCD_ILI9325D lcd;``` | ILI9325D based TFT LCD
```LCD_ILI9341 lcd;```  | ILI9341 based TFT LCD
```LCD_PCD8544 lcd;```  | PCD8544 based LCD
```LCD_SH1106 lcd;```   | SH1106 based OLED
```LCD_SSD1306 lcd;```  | SSD1306 based OLED

The library class inherits the Print class of Arduino, so that you can display texts on LCD with standard Arduino functions like this:

```C++
lcd.print("Hello, World!";)
lcd.print(foo, DEC);
lcd.print(bar, HEX);
lcd.print(1.23)         // gives "1.23" 
lcd.print(1.23456, 2);  // gives "1.23" 
```

Besides, it provides unified APIs for initializing and controlling the LCD, as well as some convenient operations.

```C++
void begin(); /* initializing */
void clear(); /* clear screen */
void setCursor(byte column, byte line); /* set current cursor, column is in pixel */
void setFontSize(FONT_SIZE size); /* set font size */
void setColor(byte r, byte g, byte b); /* set text color, not available in MicroLCD */
void setBackColor(byte r, byte g, byte b); /* set background color, not available in MicroLCD */
void printInt(uint16_t value, int8_t padding = -1); /* display a integer number */
void printLong(uint32_t value, int8_t padding = -1); /* display a long number */
void draw(const PROGMEM byte* buffer, byte width, byte height); /* draw monochrome bitmap */
```

Example
-------

```C++
#include <Wire.h>
#include <MicroLCD.h>

LCD_SSD1306 lcd; /* for SSD1306 OLED module */

static const PROGMEM uint8_t smile[48 * 48 / 8] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xE0,0xF0,0xF8,0xF8,0xFC,0xFC,0xFE,0xFE,0x7E,0x7F,0x7F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x7F,0x7F,0x7E,0xFE,0xFE,0xFC,0xFC,0xF8,0xF8,0xF0,0xE0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0xC0,0xF0,0xFC,0xFE,0xFF,0xFF,0xFF,0x3F,0x1F,0x0F,0x07,0x03,0x01,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x01,0x03,0x07,0x0F,0x1F,0x3F,0xFF,0xFF,0xFF,0xFE,0xFC,0xF0,0xC0,0x00,
0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x1F,0x1F,0x1F,0x3F,0x1F,0x1F,0x02,0x00,0x00,0x00,0x00,0x06,0x1F,0x1F,0x1F,0x3F,0x1F,0x1F,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,
0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xE0,0x00,0x00,0x30,0xF8,0xF8,0xF8,0xF8,0xE0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xE0,0xF8,0xF8,0xFC,0xF8,0x30,0x00,0x00,0xE0,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,
0x00,0x03,0x0F,0x3F,0x7F,0xFF,0xFF,0xFF,0xFC,0xF8,0xF0,0xE1,0xC7,0x87,0x0F,0x1F,0x3F,0x3F,0x3E,0x7E,0x7C,0x7C,0x7C,0x78,0x78,0x7C,0x7C,0x7C,0x7E,0x3E,0x3F,0x3F,0x1F,0x0F,0x87,0xC7,0xE1,0xF0,0xF8,0xFC,0xFF,0xFF,0xFF,0x7F,0x3F,0x0F,0x03,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x0F,0x1F,0x1F,0x3F,0x3F,0x7F,0x7F,0x7E,0xFE,0xFE,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFE,0xFE,0x7E,0x7F,0x7F,0x3F,0x3F,0x1F,0x1F,0x0F,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
};

void setup()
{
    lcd.begin();
}

void loop()
{
    lcd.clear();
    lcd.draw(smile, 48, 48);
    delay(2000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.setFontSize(FONT_SIZE_SMALL);
    lcd.print("Hello, world!");

    lcd.setCursor(0, 1);
    lcd.setFontSize(FONT_SIZE_MEDIUM);
    lcd.print("Hello, world!");

    lcd.setCursor(0, 3);
    lcd.setFontSize(FONT_SIZE_SMALL);
    lcd.printLong(12345678);

    lcd.setCursor(64, 3);
    lcd.setFontSize(FONT_SIZE_MEDIUM);
    lcd.printLong(12345678);

    lcd.setCursor(0, 4);
    lcd.setFontSize(FONT_SIZE_LARGE);
    lcd.printLong(12345678);

    lcd.setCursor(0, 6);
    lcd.setFontSize(FONT_SIZE_XLARGE);
    lcd.printLong(12345678);

    delay(3000);
}
```

Result:

![Drawing bitmap on SSD1306 OLED](http://arduinodev.com/wp-content/uploads/2013/05/oled_smile-300x247.jpg)

The following shows the real example of using MultiLCD library on a SSD1289 based 3.2" TFT LCD display.
![Freematics TFT LCD touch shield](http://freematics.com/pages/wp-content/uploads/2014/06/DSC04120.jpg)
Learn more: http://freematics.com/pages/products/obd-kit3/

Support
-------
Please use this forum for discussion or getting support - http://freematics.com/forum/viewforum.php?f=8
