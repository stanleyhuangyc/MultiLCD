MultiLCD Library for Arduino
============================

Written by Stanley Huang, distributed under GPL

This library encapsulate several libraries for various Arduino LCD shield or module into a set of unified APIs. It supports these hardware:

* DFRobot LCD4884 shield
* Nokia 5100 LCD module
* LCD1602 shield
* ZT I2C OLED module

With this library, the sketch code needs only very little change to switch from one shield to another for display.
To use a specific shield or module as the display of Arduino, all needed is to use one of following declarations:

    LCD_OLED lcd; /* for I2C OLED module */

    LCD_PCD8544 lcd; /* for LCD4884 shield or Nokia 5100 screen module */

    LCD_1602 lcd; /* for LCD1602 shield */

The library provides easy-to-use APIs for text display, including:

    void begin(); /* initializing */
    void clear(); /* clear screen */
    void setCursor(unsigned char column, unsigned char line); /* set current cursor */
    void print(const char* s); /* display a string at current cursor*/
    void printLarge(const char* s) /* display a string in large font */

