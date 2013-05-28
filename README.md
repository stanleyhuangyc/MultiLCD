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

The library includes fonts data for ASCII characters (5x7) and digits (8x8, 16x16). By using the library, it is extremely simple for display texts and numbers on desired position on a LCD screen, while very little change is needed to switch from one LCD module to another.

To use a specific shield or module as the display for Arduino, you need to include library header at the beginning of the sketch.

    #include <MultiLCD.h>

And use one of following declarations before your code.

For SSD1306 OLED module:

    LCD_SSD1306 lcd;

For LCD4884 shield or Nokia 5100 module:

    LCD_PCD8544 lcd;

For LCD1602 shield:

    LCD_1602 lcd;

For ZT I2C OLED module:

    LCD_ZTOLED lcd;

The library class inherits the Print class of Arduino, so that you can display texts on LCD with standard Arduino functions like this:

    lcd.print("Hello, World!";)
    lcd.print(foo, DEC);
    lcd.print(bar, HEX);
    lcd.print(1.23)         // gives "1.23" 
    lcd.print(1.23456, 2);  // gives "1.23" 

Besides, it provides unified APIs for initializing and controlling the LCD, as well as some convenient operations.

    void begin(); /* initializing */
    void clear(); /* clear screen */
    void setCursor(unsigned char column, unsigned char line); /* set current cursor */
    void printInt(unsigned int n, FONT_SIZE size); /* display a integer number with desired size of font*/
    void printLong(unsigned long n, FONT_SIZE size); /* display a long number with desired size of font*/
