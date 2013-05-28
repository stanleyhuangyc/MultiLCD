MultiLCD Library for Arduino
============================

Written by Stanley Huang, distributed under GPL.
For more information, please visit http://www.arduinodev.com

This library encapsulate several libraries for various Arduino LCD/LED display shields or modules into a set of unified APIs. It supports these hardware:

* DFRobot LCD4884 shield
* Nokia 3310/5100 LCD module
* LCD1602 shield
* SSD1306 OLED module
* ZT I2C OLED module

![Arduino LCD shields](http://www.arduinodev.com/wp-content/uploads/2013/03/arduino_lcd_shields-300x195.jpg)

With this library, the sketch code needs only very little change to switch from one LCD shield to another.
To use a specific shield or module as the display for Arduino, you need to include library header at the beginning of the sketch.

    #include <MultiLCD.h>

And use one of following declarations before your code.

For I2C OLED module:

    LCD_OLED lcd;
    
For LCD4884 shield or Nokia 5100 module:
    
    LCD_PCD8544 lcd;

For LCD1602 shield:

    LCD_1602 lcd;

The library provides easy-to-use APIs for text display, including:

    void begin(); /* initializing */
    void clear(); /* clear screen */
    void setCursor(unsigned char column, unsigned char line); /* set current cursor */
    void print(const char* s); /* display a string at current cursor*/
    void printLarge(const char* s) /* display a string in large font */

