#include <Arduino.h>
#include "MultiLCD.h"

/**********************************************
Define zone
**********************************************/

#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)

#define RS 59
#define WR 58
#define CS 57
#define RST 56

#define T_CLK 55
#define T_CS 60
#define T_DIN 54
#define T_DOUT 8
#define T_IRQ 9

#define X_CONST 240
#define Y_CONST 320

#define PREC_TOUCH_CONST 10

#define PixSizeX	13.78
#define PixOffsX	411

#define PixSizeY	11.01
#define PixOffsY	378

#define WINDOW_XADDR_START	0x0050 // Horizontal Start Address Set
#define WINDOW_XADDR_END	0x0051 // Horizontal End Address Set
#define WINDOW_YADDR_START	0x0052 // Vertical Start Address Set
#define WINDOW_YADDR_END	0x0053 // Vertical End Address Set
#define GRAM_XADDR		    0x0020 // GRAM Horizontal Address Set
#define GRAM_YADDR		    0x0021 // GRAM Vertical Address Set
#define GRAMWR 			    0x0022 // memory write

/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

/**********************************************
Val Zone
**********************************************/
int TP_X,TP_Y;


/**********************************************
Standard C functions zone
**********************************************/

void Write_Command(unsigned int c)
{
    byte value = *((unsigned char*)&c + 1);
    byte pe1 = (value & 0x3) | ((value & 0xC) << 2) | ((value & 0x20) >> 2);
    byte pg1 = (value & 0x10) << 1;
    byte ph1 = (value & 0xC0) >> 3;

    value = (unsigned char)c;
    byte pe2 = (value & 0x3) | ((value & 0xC) << 2) | ((value & 0x20) >> 2);
    byte pg2 = (value & 0x10) << 1;
    byte ph2 = (value & 0xC0) >> 3;

    cli();
	digitalWrite(RS,LOW);//LCD_RS=0;
	digitalWrite(CS,LOW);//LCD_CS =0;

    PORTE = pe1;
    PORTG = pg1;
    PORTH = ph1;

	digitalWrite(WR,LOW);//LCD_WR=0;
	digitalWrite(WR,HIGH);//LCD_WR=1;

    PORTE = pe2;
    PORTG = pg2;
    PORTH = ph2;

	digitalWrite(WR,LOW);//LCD_WR=0;
	digitalWrite(WR,HIGH);//LCD_WR=1;
	digitalWrite(CS,HIGH);//LCD_CS =0;
	sei();
}


void Write_Data(byte l, byte h)
{
    byte pe1 = (h & 0x3) | ((h & 0xC) << 2) | ((h & 0x20) >> 2);
    byte pg1 = (h & 0x10) << 1;
    byte ph1 = (h & 0xC0) >> 3;
    byte pe2 = (l & 0x3) | ((l & 0xC) << 2) | ((l & 0x20) >> 2);
    byte pg2 = (l & 0x10) << 1;
    byte ph2 = (l & 0xC0) >> 3;

    cli();
    digitalWrite(RS,HIGH);//LCD_RS=0;
    digitalWrite(CS,LOW);//LCD_CS =0;

    PORTE = pe1;
    PORTG = pg1;
    PORTH = ph1;

	digitalWrite(WR,LOW);//LCD_WR=0;
	digitalWrite(WR,HIGH);//LCD_WR=1;

    PORTE = pe2;
    PORTG = pg2;
    PORTH = ph2;

	digitalWrite(WR,LOW);//LCD_WR=0;
	digitalWrite(WR,HIGH);//LCD_WR=1;
	digitalWrite(CS,HIGH);//LCD_CS =0;
	sei();
}

void Write_Data(unsigned int c)
{
    byte value = *((unsigned char*)&c + 1);
    byte pe1 = (value & 0x3) | ((value & 0xC) << 2) | ((value & 0x20) >> 2);
    byte pg1 = (value & 0x10) << 1;
    byte ph1 = (value & 0xC0) >> 3;

    value = (unsigned char)c;
    byte pe2 = (value & 0x3) | ((value & 0xC) << 2) | ((value & 0x20) >> 2);
    byte pg2 = (value & 0x10) << 1;
    byte ph2 = (value & 0xC0) >> 3;

    cli();
    digitalWrite(RS,HIGH);//LCD_RS=0;
    digitalWrite(CS,LOW);//LCD_CS =0;

    PORTE = pe1;
    PORTG = pg1;
    PORTH = ph1;

	digitalWrite(WR,LOW);//LCD_WR=0;
	digitalWrite(WR,HIGH);//LCD_WR=1;

    PORTE = pe2;
    PORTG = pg2;
    PORTH = ph2;

	digitalWrite(WR,LOW);//LCD_WR=0;
	digitalWrite(WR,HIGH);//LCD_WR=1;
	digitalWrite(CS,HIGH);//LCD_CS =0;
	sei();
}

void Write_Command_Data(unsigned int cmd,unsigned int dat)
{
	Write_Command(cmd);
	Write_Data(dat);
}

void LCD_ILI9325D::begin()
{
	pinMode(RS,OUTPUT);
	pinMode(WR,OUTPUT);
	pinMode(CS,OUTPUT);
	pinMode(RST,OUTPUT);

	//DDRD = 0xFF;
    for(int a=0;a < 8;a++)
    {
        pinMode(a,OUTPUT);
    }

	digitalWrite(RST,HIGH);
    delay(1);
	digitalWrite(RST,LOW);
	delay(1);

	digitalWrite(RST,HIGH);
	digitalWrite(CS,HIGH);
	digitalWrite(WR,HIGH);
	delay(20);


	Write_Command_Data(0x0001,0x0100);
	Write_Command_Data(0x0002,0x0700);
	Write_Command_Data(0x0003,0x1030);
	Write_Command_Data(0x0004,0x0000);
	Write_Command_Data(0x0008,0x0207);
	Write_Command_Data(0x0009,0x0000);
	Write_Command_Data(0x000A,0x0000);
	Write_Command_Data(0x000C,0x0000);
	Write_Command_Data(0x000D,0x0000);
	Write_Command_Data(0x000F,0x0000);
	//power on sequence VGHVGL
	Write_Command_Data(0x0010,0x0000);
	Write_Command_Data(0x0011,0x0007);
	Write_Command_Data(0x0012,0x0000);
	Write_Command_Data(0x0013,0x0000);
	//vgh
	Write_Command_Data(0x0010,0x1290);
	Write_Command_Data(0x0011,0x0227);
	//delays(100);
	//vregiout
	Write_Command_Data(0x0012,0x001d); //0x001b
	//delays(100);
	//vom amplitude
	Write_Command_Data(0x0013,0x1500);
	//delays(100);
	//vom H
	Write_Command_Data(0x0029,0x0018);
	Write_Command_Data(0x002B,0x000D);

	//gamma
	Write_Command_Data(0x0030,0x0004);
	Write_Command_Data(0x0031,0x0307);
	Write_Command_Data(0x0032,0x0002);// 0006
	Write_Command_Data(0x0035,0x0206);
	Write_Command_Data(0x0036,0x0408);
	Write_Command_Data(0x0037,0x0507);
	Write_Command_Data(0x0038,0x0204);//0200
	Write_Command_Data(0x0039,0x0707);
	Write_Command_Data(0x003C,0x0405);// 0504
	Write_Command_Data(0x003D,0x0F02);
	//ram
	Write_Command_Data(0x0050,0x0000);
	Write_Command_Data(0x0051,0x00EF);
	Write_Command_Data(0x0052,0x0000);
	Write_Command_Data(0x0053,0x013F);
	Write_Command_Data(0x0060,0xA700);
	Write_Command_Data(0x0061,0x0001);
	Write_Command_Data(0x006A,0x0000);
	//
	Write_Command_Data(0x0080,0x0000);
	Write_Command_Data(0x0081,0x0000);
	Write_Command_Data(0x0082,0x0000);
	Write_Command_Data(0x0083,0x0000);
	Write_Command_Data(0x0084,0x0000);
	Write_Command_Data(0x0085,0x0000);
	//
	Write_Command_Data(0x0090,0x0010);
	Write_Command_Data(0x0092,0x0600);
	Write_Command_Data(0x0093,0x0003);
	Write_Command_Data(0x0095,0x0110);
	Write_Command_Data(0x0097,0x0000);
	Write_Command_Data(0x0098,0x0000);
	Write_Command_Data(0x0007,0x0133);

	m_color = 0xffff;
	m_x = 0;
	m_y = 0;
}

void SetXY(unsigned int x0,unsigned int x1,unsigned int y0,unsigned int y1)
{
  Write_Command_Data(WINDOW_XADDR_START,x0);
  Write_Command_Data(WINDOW_XADDR_END,x1);
  Write_Command_Data(WINDOW_YADDR_START,y0);
  Write_Command_Data(WINDOW_YADDR_END,y1);
  Write_Command_Data(GRAM_XADDR,x0);
  Write_Command_Data(GRAM_YADDR,y0);
  Write_Command (0x0022);//LCD_WriteCMD(GRAMWR);
}

void Pant(unsigned int color)
{
	int i,j;
	SetXY(0,239,0,319);

    for(i=0;i<320;i++)
	 {
	  for (j=0;j<240;j++)
	   	{
         Write_Data(color);
	    }

	  }
}

void LCD_ILI9325D::clear()
{
    Pant(0);
}

size_t LCD_ILI9325D::write(uint8_t c)
{
    byte pgm_buffer[6];
    memcpy_P(pgm_buffer, &font5x8[(c < ' ' || c > 0x7f) ? 0 : c - ' '], sizeof(pgm_buffer));
    pgm_buffer[5] = 0;

    SetXY(m_x, m_x + 7, m_y, m_y + 5);
    for (byte i = 0; i < 5; i++) {
        unsigned char d = pgm_buffer[i];
        for (byte j = 0; j < 8; j++) {
            Write_Data((d & 1 ) ? m_color : 0);
            d >>= 1;
        }
    }
    m_y += 6;
}

void LCD_ILI9325D::writeDigit(byte n, FONT_SIZE size)
{
    if (size == FONT_SIZE_SMALL) {
        byte pgm_buffer[6] = {0};
        if (n >= 0 && n <= 9) {
            memcpy_P(pgm_buffer, &font5x8[n + '0' - ' '], 5);
        }

        SetXY(m_x, m_x + 7, m_y, m_y + 5);
        for (byte i = 0; i < 5; i++) {
            unsigned char d = pgm_buffer[i];
            for (byte j = 0; j < 8; j++) {
                Write_Data((d & 1 ) ? m_color : 0);
                d >>= 1;
            }
        }
        m_y += 6;
    } else if (size == FONT_SIZE_MEDIUM) {
        byte pgm_buffer[8];
        if (n >= 0 && n <= 9) {
            memcpy_P(pgm_buffer, &digits8x8[n], 8);
        } else {
            memset(pgm_buffer, 0, 8);
        }

        SetXY(m_x, m_x + 7, m_y, m_y + 7);
        for (byte i = 0; i < 8; i++) {
            unsigned char d = pgm_buffer[i];
            for (byte j = 0; j < 8; j++) {
                Write_Data((d & 1 ) ? m_color : 0);
                d >>= 1;
            }
        }
        m_y += 8;
    } else if (size == FONT_SIZE_LARGE) {
        byte pgm_buffer[32];
        if (n >= 0 && n <= 9) {
            memcpy_P(pgm_buffer, &digits16x16[n], 32);
        } else {
            memset(pgm_buffer, 0, sizeof(pgm_buffer));
        }

        SetXY(m_x, m_x + 15, m_y, m_y + 15);
        for (byte i = 0; i < 16; i++) {
            unsigned char d = pgm_buffer[i];
            for (byte j = 0; j < 8; j++) {
                Write_Data((d & 1 ) ? m_color : 0);
                d >>= 1;
            }
            d = pgm_buffer[i + 16];
            for (byte j = 0; j < 8; j++) {
                Write_Data((d & 1 ) ? m_color : 0);
                d >>= 1;
            }
        }
        m_y += 16;
    }
}

void Print16x24(int x, int y, unsigned int color, const char* data)
{
    SetXY(x, x + 23, y, y + 15);
    for (int i = 0; i < 48; i++) {
        unsigned char d = data[i];
        for (int j = 0; j < 8; j++) {
            Write_Data((d & 1 ) ? color : 0);
            d >>= 1;
        }
    }
}

void Print8x16(int x, int y, unsigned int color, const char* data)
{
    SetXY(x, x + 7, y, y + 15);
    for (int i = 0; i < 16; i++) {
        unsigned char d = *data;
        for (int j = 0; j < 8; j++) {
            Write_Data((d & 1 ) ? color : 0);
            d >>= 1;
        }
        data++;
    }

}

void Touch_Init(void)
{
	pinMode(T_CLK,  OUTPUT);
    pinMode(T_CS,   OUTPUT);
    pinMode(T_DIN,  OUTPUT);
    pinMode(T_DOUT, INPUT);
    pinMode(T_IRQ,  INPUT);

	digitalWrite(T_CS,  HIGH);
	digitalWrite(T_CLK, HIGH);
	digitalWrite(T_DIN, HIGH);
	digitalWrite(T_CLK, HIGH);
}

void Touch_WriteData(unsigned char data)
{
	unsigned char temp;
	unsigned char nop;
	unsigned char count;

	temp=data;
	digitalWrite(T_CLK,LOW);

	for(count=0; count<8; count++)
	{
		if(temp & 0x80)
			digitalWrite(T_DIN, HIGH);
		else
			digitalWrite(T_DIN, LOW);
		temp = temp << 1;
		digitalWrite(T_CLK, LOW);
		nop++;
		digitalWrite(T_CLK, HIGH);
		nop++;
	}
}

unsigned int Touch_ReadData()
{
	unsigned char nop;
	unsigned int data = 0;
	unsigned char count;
	for(count=0; count<12; count++)
	{
		data <<= 1;
		digitalWrite(T_CLK, HIGH);
		nop++;
		digitalWrite(T_CLK, LOW);
		nop++;
		if (digitalRead(T_DOUT))
			data++;
	}
	return(data);
}
void Touch_Read()
{
	unsigned long tx=0;
	unsigned long ty=0;

	digitalWrite(T_CS,LOW);

	for (int i=0; i<PREC_TOUCH_CONST; i++)
	{
		Touch_WriteData(0x90);
		digitalWrite(T_CLK,HIGH);
		digitalWrite(T_CLK,LOW);
		ty+=Touch_ReadData();

		Touch_WriteData(0xD0);
		digitalWrite(T_CLK,HIGH);
		digitalWrite(T_CLK,LOW);
		tx+=Touch_ReadData();
	}

	digitalWrite(T_CS,HIGH);

	TP_X=tx/PREC_TOUCH_CONST;
	TP_Y=ty/PREC_TOUCH_CONST;
}

char Touch_DataAvailable()
{
  char avail;
  avail = !digitalRead(T_IRQ);
  return avail;
}

int Touch_GetX()
{
	int value;
	value = ((TP_X-PixOffsX)/PixSizeX);
	if (value < 0)
		value = 0;
	return value;
}

int Touch_GetY()
{
	int value;
	value = ((TP_Y-PixOffsY)/PixSizeY);
	if (value < 0)
		value = 0;
	return value;
}

#endif
