/*************************************************************************
* Arduino Text Display Library for Multiple LCDs
* Distributed under GPL v2.0
* Copyright (c) 2013 Stanley Huang <stanleyhuangyc@live.com>
* All rights reserved.
*************************************************************************/

typedef enum {
    FONT_SIZE_SMALL = 0,
    FONT_SIZE_MEDIUM,
    FONT_SIZE_LARGE,
    FONT_SIZE_XLARGE
} FONT_SIZE;


extern const PROGMEM unsigned char font5x8[][5];
extern const PROGMEM unsigned char digits8x8[][8] ;
extern const PROGMEM unsigned char digits16x16[][32];

#include "PCD8544.h"

class LCD_Common
{
public:
    virtual void backlight(bool on) {}
    virtual byte getLines() = 0;
    virtual byte getCols() = 0;
    virtual void changeLine() {}
    virtual void draw(const PROGMEM byte* buffer, byte x, byte y, byte width, byte height) {}
    void printInt(unsigned int value, FONT_SIZE size, byte padding = 0);
    void printLong(unsigned long value, FONT_SIZE size, byte padding = 0);
protected:
    virtual void writeDigit(byte n, FONT_SIZE size) {}
};

class LCD_PCD8544 : public LCD_Common, public PCD8544
{
public:
    byte getLines() { return 6; }
    byte getCols() { return 14; }
    void backlight(bool on)
    {
        pinMode(7, OUTPUT);
        digitalWrite(7, on ? HIGH : LOW);
    }
    void clearLine(byte line)
    {
        setCursor(0, line);
        for (byte i = 14; i > 0; i--) write(' ');
    }
    void changeLine()
    {
        column = 0;
        line ++;
    }
    void printLarge(const char* s);
private:
    void writeDigit(byte n, FONT_SIZE size = FONT_SIZE_MEDIUM);
};

#include "ZtLib.h"

#define OLED_ADDRESS 0x27

class LCD_ZTOLED : public LCD_Common, public ZtLib, public Print
{
public:
    byte getLines() { return 4; }
    byte getCols() { return 16; }
    void setCursor(byte column, byte line);
    void changeLine()
    {
        m_column = 0;
        m_page += 2;
    }
    size_t write(uint8_t c);
    void print(const char* s);
    void writeDigit(byte n, FONT_SIZE size = FONT_SIZE_MEDIUM);
    void clear();
    void begin();
    void backlight(bool on) {}
    void clearLine(byte line)
    {
        setCursor(0, line);
        for (byte i = 16; i > 0; i--) write(' ');
    }
private:
    unsigned char m_column;
    unsigned char m_page;
};

#include "LCD4Bit_mod.h"
class LCD_1602 : public LCD_Common, public LCD4Bit_mod
{
public:
    byte getLines() { return 2; }
    byte getCols() { return 16; }
    void writeDigit(byte n, FONT_SIZE size = FONT_SIZE_SMALL)
    {
        if (size == FONT_SIZE_SMALL)
            write(n >= 0 && n <= 9 ? '0' + n : ' ');
    }
    void clearLine(byte line)
    {
        setCursor(0, line);
        for (byte i = 16; i > 0; i--) write(' ');
    }
};

#include "SSD1306.h"

class LCD_SSD1306 : public LCD_Common, public SSD1306, public Print
{
public:
    void setCursor(byte column, byte line);
    void draw(const PROGMEM byte* buffer, byte x, byte y, byte width, byte height);
    size_t write(uint8_t c);
    void clear()
    {
        fill(0);
    }
    byte getLines() { return 21; }
    byte getCols() { return 8; }
private:
    void writeDigit(byte n, FONT_SIZE size);
    byte m_col;
    byte m_row;
};

class LCD_ILI9325D : public LCD_Common, public Print
{
public:
    void setCursor(byte column, byte line)
    {
        m_y = column * 6;
        m_x = line * 16;
    }
    void setColor(uint16_t color)
    {
        m_color = color;
    }
    void begin();
    void clear();
    size_t write(uint8_t);
    byte getLines() { return 53; }
    byte getCols() { return 30; }
private:
    void writeDigit(byte n, FONT_SIZE size);
    int m_x;
    int m_y;
    uint16_t m_color;
};
