/*************************************************************************
* Demo sketch of Arduino Text Display Library for Multiple LCDs
* Distributed under GPL v2.0
* Copyright (c) 2013 Stanley Huang <stanleyhuangyc@live.com>
* All rights reserved.
*************************************************************************/

#include <Arduino.h>
#include <Wire.h>
#include <MultiLCD.h>

LCD_SSD1306 lcd; /* for SSD1306 OLED module */
//LCD_PCD8544 lcd; /* for LCD4884 shield or Nokia 5100 screen module */
//LCD_1602 lcd; /* for LCD1602 shield */
//LCD_ZTOLED lcd; /* for ZT OLED module */

void setup()
{
	lcd.begin();
	lcd.clear();

	lcd.setCursor(0, 0);
	lcd.print("Hello, world!");

	lcd.setCursor(0, 1);
	lcd.printLong(1234567890, FONT_SIZE_SMALL);

	lcd.setCursor(0, 2);
	lcd.printLong(1234567890, FONT_SIZE_MEDIUM);

	lcd.setCursor(0, 3);
	lcd.printLong(12345678, FONT_SIZE_LARGE);

	//pinMode(8, INPUT);
}

void loop()
{
    /*
  	lcd.setCursor(0, 7);
    if (digitalRead(8)) {
        lcd.write('+');
    } else {
        lcd.write('-');
    }
    */
}
