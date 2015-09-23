#include <Arduino.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include "obdhud.h"
#include "RGBLED.h"

/**************************************************
define the extern data zone
**************************************************/
extern byte font5_8[][5];
//extern byte pic[4][8][8][3];
extern CONFIG_DATA cfgdata;
extern uint16_t EEMEM eepColorData[][16];
extern CONTEXT ctx;

/********************************************************
Name:DispShowChar
Function:Display a English latter in LED matrix
Parameter:chr :the latter want to show
          R: the value of RED.   Range:RED 0~255
          G: the value of GREEN. Range:RED 0~255
          B: the value of BLUE.  Range:RED 0~255
          bias: the bias of a letter in LED Matrix.Range -7~7
********************************************************/
void display(char c, char bias_x, char bias_y, byte color[])
{
    byte i;
    byte to;
    byte mask;
    int offset = c - '0';
    if (bias_x <= 0) {
        i = 0;
        to = bias_x +5;
    } else {
        i = bias_x;
        to = min(bias_x +5, 8);
    }
    mask = offset >= 0 ? 0xff : 0;
    for (; i < to; i++) {
        byte j = 0;
        byte temp = pgm_read_byte(&(font5_8[offset][i - bias_x])) & mask;
        if (bias_y < 0) {
            temp <<= (-bias_y);
        } else {
            for (; j < bias_y; j++) {
                ClearPixel(i, j);
            }
        }
        for(;j < 8;j++) {
          if(temp & 0x80) {
            SetPixel(i, j, color);
          } else {
            ClearPixel(i, j);
          }
          temp <<= 1;
        }
    }
}

/********************************************************
Name:DispShowColor
Function:Fill a color in LED matrix
Parameter:R: the value of RED.   Range:RED 0~255
          G: the value of GREEN. Range:RED 0~255
          B: the value of BLUE.  Range:RED 0~255
******************************.**************************/
void DispShowColor(byte color[])
{
    byte i,j;
    for (i = 0;i<8;i++) {
        for(j = 0;j<8;j++) {
          SetPixel(i, j, color);
        }
    }
}

#define COLOR12_R(c) ((uint8_t)(c >> 4) | 0xf)
#define COLOR12_G(c) ((uint8_t)c | 0xf)
#define COLOR12_B(c) (((uint8_t)c << 4) | 0xf)

uint8_t limit8(int val)
{
    uint8_t v;
    if (val < 0)
        v = 0;
    else if (val > 255)
        v = 255;
    else
        v = (uint8_t)val;
    return v;
}

uint8_t* GetColor(uint8_t index, uint8_t offset, uint8_t color[])
{
    uint16_t c = eeprom_read_word(&eepColorData[index][offset]);
    color[0] = COLOR12_R(c);
    color[1] = COLOR12_G(c);
    color[2] = COLOR12_B(c);
    return color;
}

void GetRGB8(uint8_t index, uint8_t val, uint8_t color[])
{
    byte offset = val >> 4;
    byte n = val & 0xf;
    GetColor(index, offset, color);
    if (offset == 15)
        return;

    byte color2[3];
    GetColor(index, offset + 1, color2);
    color[0] = ((uint16_t)color[0] * (0xf - n) + (uint16_t)color2[0] * n) >> 4;
    color[1] = ((uint16_t)color[1] * (0xf - n) + (uint16_t)color2[1] * n) >> 4;
    color[2] = ((uint16_t)color[2] * (0xf - n) + (uint16_t)color2[2] * n) >> 4;
}

void GetRGB(uint8_t index, uint16_t val, uint8_t color[])
{
    uint16_t offset = val >> 12;
    uint16_t n = (val >> 4) & 0xff;

    GetColor(index, offset, color);
    if (offset == 15)
        return;

    byte color2[3];
    GetColor(index, offset + 1, color2);
    color[0] = ((uint16_t)color[0] * (0xff - n) + (uint16_t)color2[0] * n) >> 8;
    color[1] = ((uint16_t)color[1] * (0xff - n) + (uint16_t)color2[1] * n) >> 8;
    color[2] = ((uint16_t)color[2] * (0xff - n) + (uint16_t)color2[2] * n) >> 8;
}

#if 0
void GetRGB(int val, uint8_t color[])
{
    COLOR_INDEX cur;
    COLOR_INDEX last;
    uint8_t i = 1;
    GetDigitColor(&last, 0);
    for (;;) {
        GetDigitColor(&cur, i);
        if (val < cur.value) {
            int range = cur.value - last.value;
            int value = val - last.value;
            color[0] = COLOR12_R(last.color);
            color[1] = COLOR12_G(last.color);
            color[2] = COLOR12_B(last.color);
            color[0] = ((int)COLOR12_R(cur.color) - color[0]) * value / range + color[0];
            color[1] = ((int)COLOR12_G(cur.color) - color[1]) * value / range + color[1];
            color[2] = ((int)COLOR12_B(cur.color) - color[2]) * value / range + color[2];
            return;
        }
        if (++i == MAX_COLOR_ENTRIES) {
            color[0] = COLOR12_R(cur.color);
            color[1] = COLOR12_G(cur.color);
            color[2] = COLOR12_B(cur.color);
            return;
        }
        last.color = cur.color;
        last.value = cur.value;
    }
}
#endif

void SetFadePixel(uint8_t x, uint8_t y, uint8_t color[], uint8_t shift)
{
    byte fadecolor[] = {color[0] >> shift, color[1] >> shift, color[2] >> shift};
    SetPixel(x, y, fadecolor);
}

void SetCrossFadePixel(uint8_t x, uint8_t y, uint8_t color[], uint8_t color2[], uint8_t shift)
{
    byte fadecolor[] = {
        (color[0] >> shift) + (color2[0] >> (8 - shift)),
        (color[1] >> shift) + (color2[1] >> (8 - shift)),
        (color[2] >> shift) + (color2[2] >> (8 - shift))
    };
    SetPixel(x, y, fadecolor);
}

void displayVBar(uint8_t val, uint8_t x)
{
    uint8_t y;
    if (val == 0) {
        for (y = 0; y < 8; y++) {
            ClearPixel(x, y);
        }
    } else {
        uint8_t m = 8 - ((val & 0xf) >> 1); /* 8~1 */
        uint8_t n = (val & 0x7f) >> 4;
        uint8_t offset = (val & 0x80) >> 4;
        byte color[3];
        for (y = 0; y < n; y++) {
            GetColor(cfgdata.barColorIndex, y + offset, color);
            SetPixel(x, y, color);
        }
        if (offset) {
            byte colorbg[3];
            GetColor(cfgdata.barColorIndex, y + offset, color);
            GetColor(cfgdata.barColorIndex, y, colorbg);
            SetCrossFadePixel(x, y, color, colorbg, m);
            while (++y < 8) {
                SetPixel(x, y, colorbg);
            }
        } else {
            GetColor(cfgdata.barColorIndex, y + offset, color);
            SetFadePixel(x, y, color, m);
            while (++y < 8) {
                ClearPixel(x, y);
            }
        }
    }
}

void displayHBar(uint8_t val, uint8_t y, uint8_t style)
{
    uint8_t xmask = 0;
    int8_t xoff = 0;
    int8_t x;
    uint8_t m, n, offset;
    byte color[3];
    byte colorbg[3];

    if (val == 0) {
        for (x = 0; x < 8; x++) {
            ClearPixel(x, y);
        }
        return;
    }

    switch (style) {
    case BAR_CENTER:
        m = 8 - ((val & 0x1f) >> 2); /* 8~1 */
        n = (val & 0x7f) >> 5;
        offset = (val & 0x80) >> 4;
        for (x = 0; x < n; x++) {
            SetPixel(3 - x, y, GetColor(cfgdata.barColorIndex, offset + 3 - x, color));
            SetPixel(4 + x, y, GetColor(cfgdata.barColorIndex, offset + 4 + x, color));
        }
        if (offset) {
            SetCrossFadePixel(3 - x, y, GetColor(cfgdata.barColorIndex, offset + 3 - x, color), GetColor(cfgdata.barColorIndex, 3 - x, colorbg), m);
            SetCrossFadePixel(4 + x, y, GetColor(cfgdata.barColorIndex, offset + 4 + x, color), GetColor(cfgdata.barColorIndex, 4 + x, colorbg), m);
            while (++x < 4) {
                SetPixel(3 - x, y, GetColor(cfgdata.barColorIndex, 3 - x, color));
                SetPixel(4 + x, y, GetColor(cfgdata.barColorIndex, 4 + x, color));
            }
        } else {
            SetFadePixel(3 - x, y, GetColor(cfgdata.barColorIndex, offset + 3 - x, color), m);
            SetFadePixel(4 + x, y, GetColor(cfgdata.barColorIndex, offset + 4 + x, color), m);
            while (++x < 4) {
                ClearPixel(3 - x, y);
                ClearPixel(4 + x, y);
            }
        }
        break;
    case BAR_RIGHT:
        // mirror
        xmask = 0xff;
        xoff = 8;
    case BAR_LEFT:
        m = 8 - ((val & 0xf) >> 1); /* 8~1 */
        n = (val & 0x7f) >> 4;
        offset = (val & 0x80) >> 4;
        for (x = 0; x < n; x++) {
            SetPixel((x ^ xmask) + xoff, y, GetColor(cfgdata.barColorIndex, x + offset, color));
        }
        if (offset) {
            SetCrossFadePixel((x ^ xmask) + xoff, y,
                GetColor(cfgdata.barColorIndex, x + offset, color), GetColor(cfgdata.barColorIndex, x, colorbg), m);
            while (++x < 8) {
                SetPixel((x ^ xmask) + xoff, y, GetColor(cfgdata.barColorIndex, x, color));
            }
        } else {
            SetFadePixel((x ^ xmask) + xoff, y, GetColor(cfgdata.barColorIndex, x + offset, color), m);
            while (++x < 8) {
                ClearPixel((x ^ xmask) + xoff, y);
            }
        }
        break;
    case BAR_ZERO_CENTER:
        if (val >= 128) {
            val -= 128;
            m = 8 - ((val & 0x1f) >> 2); /* 8~1 */
            n = val >> 5;
            for (x = 0; x < n; x++) {
                SetPixel(4 + x, y, GetColor(cfgdata.barColorIndex, 4 + x, color));
            }
            SetFadePixel(4 + x, y, GetColor(cfgdata.barColorIndex, 4 + x, color), m);
            while (++x < 4) ClearPixel(4 + x, y);
        } else {
            val = 128 - val;
            m = 8 - ((val & 0x1f) >> 2); /* 8~1 */
            n = val >> 5;
            for (x = 0; x < n; x++) {
                SetPixel(3 - x, y, GetColor(cfgdata.barColorIndex, 3 - x, color));
            }
            SetFadePixel(3 - x, y, GetColor(cfgdata.barColorIndex, 3 - x, color), m);
            while (++x < 4) ClearPixel(3 - x, y);
        }
        break;
    }
}

#if 0
void display20(uint8_t val, int8_t bais_y, uint8_t color[])
{
    char c;
    if (val >= 10) {
        if (val > 19)
            c = '?';
        else
            c = '0' + (val - 10);
        val = 1;
    } else {
        c = '0' + val;
        val = 0;
    }

    display(val ? '1' : 0, -1, bais_y, color);
    display(c, 3, bais_y, color);
}
#endif

void DrawLineH(byte x, byte y, byte len, byte color[])
{
    byte i;
    for (i = 0; i < len; i++) {
        SetPixel(x + i, y, color);
    }
}

void DrawLineV(byte x, byte y, byte len, byte color[])
{
    byte i;
    for (i = 0; i < len; i++) {
        SetPixel(x, y + i, color);
    }
}

void displayDigit3x5(byte val, char bias_x, char bias_y, byte color[])
{
    switch (val) {
    case 0:
        SetPixel(bias_x + 1, bias_y, color);
        SetPixel(bias_x + 1, bias_y + 4, color);
        DrawLineV(bias_x, bias_y, 5, color);
        DrawLineV(bias_x + 2, bias_y, 5, color);
        break;
    case 1:
        DrawLineV(bias_x + 1, bias_y, 5, color);
        break;
    case 2:
        DrawLineH(bias_x, bias_y, 3, color);
        DrawLineH(bias_x, bias_y + 2, 3, color);
        DrawLineH(bias_x, bias_y + 4, 3, color);
        SetPixel(bias_x, bias_y + 1, color);
        SetPixel(bias_x + 2, bias_y + 3, color);
        break;
    case 3:
        DrawLineV(bias_x + 2, bias_y, 5, color);
        DrawLineH(bias_x, bias_y, 2, color);
        DrawLineH(bias_x, bias_y + 2, 2, color);
        DrawLineH(bias_x, bias_y + 4, 2, color);
        break;
    case 4:
        DrawLineV(bias_x, bias_y + 2, 3, color);
        DrawLineV(bias_x + 2, bias_y, 5, color);
        SetPixel(bias_x + 1, bias_y + 2, color);
        break;
    case 5:
        DrawLineH(bias_x, bias_y, 3, color);
        DrawLineH(bias_x, bias_y + 2, 3, color);
        DrawLineH(bias_x, bias_y + 4, 3, color);
        SetPixel(bias_x + 2, bias_y + 1, color);
        SetPixel(bias_x, bias_y + 3, color);
        break;
    case 6:
        DrawLineV(bias_x, bias_y, 5, color);
        DrawLineV(bias_x + 2, bias_y, 3, color);
        SetPixel(bias_x + 1, bias_y, color);
        SetPixel(bias_x + 1, bias_y + 2, color);
        DrawLineH(bias_x + 1, bias_y + 4, 2, color);
        break;
    case 7:
        DrawLineV(bias_x + 2, bias_y, 5, color);
        DrawLineH(bias_x, bias_y + 4, 2, color);
        break;
    case 8:
        DrawLineV(bias_x, bias_y, 5, color);
        DrawLineV(bias_x + 2, bias_y, 5, color);
        SetPixel(bias_x + 1, bias_y, color);
        SetPixel(bias_x + 1, bias_y + 2, color);
        SetPixel(bias_x + 1, bias_y + 4, color);
        break;
    case 9:
        DrawLineV(bias_x + 2, bias_y, 5, color);
        DrawLineV(bias_x, bias_y + 2, 3, color);
        SetPixel(bias_x + 1, bias_y + 4, color);
        SetPixel(bias_x + 1, bias_y + 2, color);
        DrawLineH(bias_x, bias_y, 2, color);
        break;
    }
}

void displayDigit3x7(byte val, char bias_x, char bias_y, byte color[])
{
    switch (val) {
    case 0:
        SetPixel(bias_x + 1, bias_y, color);
        SetPixel(bias_x + 1, bias_y + 6, color);
        DrawLineV(bias_x, bias_y, 7, color);
        DrawLineV(bias_x + 2, bias_y, 7, color);
        break;
    case 1:
        DrawLineV(bias_x + 1, bias_y, 7, color);
        break;
    case 2:
        DrawLineH(bias_x, bias_y, 3, color);
        DrawLineH(bias_x, bias_y + 3, 3, color);
        DrawLineH(bias_x, bias_y + 6, 3, color);
        DrawLineV(bias_x, bias_y + 1, 2, color);
        DrawLineV(bias_x + 2, bias_y + 4, 2, color);
        break;
    case 3:
        DrawLineV(bias_x + 2, bias_y, 7, color);
        DrawLineH(bias_x, bias_y, 2, color);
        DrawLineH(bias_x, bias_y + 3, 2, color);
        DrawLineH(bias_x, bias_y + 6, 2, color);
        break;
    case 4:
        DrawLineV(bias_x, bias_y + 3, 4, color);
        DrawLineV(bias_x + 2, bias_y, 7, color);
        SetPixel(bias_x + 1, bias_y + 3, color);
        break;
    case 5:
        DrawLineH(bias_x, bias_y, 3, color);
        DrawLineH(bias_x, bias_y + 3, 3, color);
        DrawLineH(bias_x, bias_y + 6, 3, color);
        DrawLineV(bias_x + 2, bias_y + 1, 2, color);
        DrawLineV(bias_x, bias_y + 4, 2, color);
        break;
    case 6:
        DrawLineV(bias_x, bias_y, 7, color);
        DrawLineV(bias_x + 2, bias_y, 4, color);
        SetPixel(bias_x + 1, bias_y, color);
        SetPixel(bias_x + 1, bias_y + 3, color);
        DrawLineH(bias_x + 1, bias_y + 6, 2, color);
        break;
    case 7:
        DrawLineV(bias_x + 2, bias_y, 7, color);
        DrawLineH(bias_x, bias_y + 6, 2, color);
        SetPixel(bias_x, bias_y + 5, color);
        break;
    case 8:
        DrawLineV(bias_x, bias_y, 7, color);
        DrawLineV(bias_x + 2, bias_y, 7, color);
        SetPixel(bias_x + 1, bias_y, color);
        SetPixel(bias_x + 1, bias_y + 3, color);
        SetPixel(bias_x + 1, bias_y + 6, color);
        break;
    case 9:
        DrawLineV(bias_x + 2, bias_y, 7, color);
        DrawLineV(bias_x, bias_y + 3, 4, color);
        SetPixel(bias_x + 1, bias_y + 6, color);
        SetPixel(bias_x + 1, bias_y + 3, color);
        DrawLineH(bias_x, bias_y, 2, color);
        break;
    }
}

void FillUp(byte value, byte color[], char mirrored)
{
    byte n = value % 8;
    byte m = value >> 3;
    byte x;
    byte y = 0;

    for (y = 0; y < m; y++) {
        for (x = 0; x < 8; x++) {
            SetPixel(x, y, color);
        }
    }
    for (x = 0; x < n; x++) {
        SetPixel(mirrored ? 7 - x : x, y, color);
    }
}
