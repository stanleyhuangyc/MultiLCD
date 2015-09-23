#include <Arduino.h>
#include <avr/pgmspace.h>
#include "RGBMatrix.h"

byte pageIndex = 0; // the index of buffer
char xoff ,xmask, yoff, ymask;

#ifndef RAINBOW
/*****************************
define the operate commands
*****************************/

/*****************************
define the status
*****************************/

/*****************************
define the IO
*****************************/
#define RST_BIT 0x04
#define LAT_BIT 0x02
#define SLB_BIT 0x01
#define SCL_BIT 0x40
#define SDA_BIT 0x80

#define RST PORTC
#define LAT PORTC
#define SLB PORTC
#define SDA PORTD
#define SCL PORTD

#define open_line0	{PORTB=0x01;}
#define open_line1	{PORTB=0x02;}
#define open_line2	{PORTB=0x04;}
#define open_line3	{PORTB=0x08;}
#define open_line4	{PORTB=0x10;}
#define open_line5	{PORTB=0x20;}
#define open_line6	{PORTD=0x08;}
#define open_line7	{PORTD=0x10;}
#define close_all_line	{PORTD=0x00;PORTB=0x00;}

/*******************************************
define the data zone
*******************************************/
byte dots[2][8][8][3];
//dots matrix
//[2]:Page:one for display, one for receive data
//[8]:Row:8 row in LED plane
//[8]:Column:8 column in one row
//[3]:Color:RGB data: 0 for Red; 1 for green, 2 for Blue
byte gammaValues[3] = {15,63,63};
//Gamma correctly value, every LED plane is different.value range is 0~63
//[3]:RGB data, 0 for Red; 1 for green, 2 for Blue
byte row = 0;//the value of row in LED plane, from 0~7
byte column = 0;//the value of every row, from 0~7
byte color = 0;//the value of every dots, 0 is Red, 1 is Green, 2 is Blue
byte line = 0;

/***************************************************
all parts inition functions zone
***************************************************/
void LED_RST(byte temp);
void LED_Delay(byte i);
void run(byte k);
void open_line(byte x);
void SetGamma();

void LED_Delay(byte i)
{
  unsigned int y;
  y = i * 10;
  while(y--);
}

void _IO_Init()
{
  DDRD = 0xff; // set all pins direction of PortD
  DDRC = 0xff; // set all pins direction of PortC
  DDRB = 0xff; // set all pins direction of PortB

  PORTD = 0x00; // set all pins output is low of PortD
  PORTC = 0x00; // set all pins output is low of PortC
  PORTB = 0x00; // set all pins output is low of PortB
}
void _LED_Init()
{
  LED_RST(1);
  LED_Delay(1);
  LED_RST(0);
  LED_Delay(1);
  LED_RST(1);
  LED_Delay(1);

  SetGamma();
  line = 0;

}
void _TC2_Init()
{
  TCCR2A |= (1 << WGM21) | (1 << WGM20);
  TCCR2B |= ((1<<CS22)|(1<<CS20));   // by clk/64
  TCCR2B &= ~((1<<CS21));   // by clk/64
  TCCR2A &= ~((1<<WGM21) | (1<<WGM20));   // Use normal mode
  ASSR |= (1<<AS2);       // Use internal clock - external clock not used in Arduino
  TIMSK2 |= (1<<TOIE2) | (0<<OCIE2B);   //Timer2 Overflow Interrupt Enable
  TCNT2 = 0xff;
  sei();
}


/****************************************************
the LED Hardware operate functions zone
****************************************************/
void LED_SDA(byte temp)
{
  if (temp)
    SDA|=SDA_BIT;
  else
    SDA&=~SDA_BIT;
}

void LED_SCL(byte temp)
{
  if (temp)
    SCL|=SCL_BIT;
  else
    SCL&=~SCL_BIT;
}

void LED_RST(byte temp)
{
  if (temp)
    RST|=RST_BIT;
  else
    RST&=~RST_BIT;
}

void LED_LAT(byte temp)
{
  if (temp)
    LAT|=LAT_BIT;
  else
    LAT&=~LAT_BIT;
}

void LED_SLB(byte temp)
{
  if (temp)
    SLB|=SLB_BIT;
  else
    SLB&=~SLB_BIT;
}
/***************************************************
the LED datas operate functions zone
***************************************************/
void SetGamma()
{
  byte i = 0;
  byte j = 0;
  byte k = 0;
  byte temp = 0;
  LED_LAT(0);
  LED_SLB(0);
  for(k=0;k<8;k++)
    for(i = 3;i > 0 ;i--)
    {
      temp = gammaValues[i-1]<<2;
      for(j = 0;j<6;j++)
      {
        if(temp &0x80)
          LED_SDA(1);
        else
          LED_SDA(0);

        temp =temp << 1;
        LED_SCL(0);
        LED_SCL(1);
    }
  }
  LED_SLB(1);
}
void run(byte k)
{
  byte i = 0;
  byte j = 0;
  byte p = 0;
  byte temp = 0;
  LED_SLB(1);
  LED_LAT(0);
  for(i = 0;i<8;i++)
  {

    for(j=0;j<3;j++)
    {
      temp = dots[pageIndex][k][i][j];
      for(p=0;p<8;p++)
      {
         if(temp & 0x80)
           LED_SDA(1);
         else
           LED_SDA(0);

         temp = temp<<1;
         LED_SCL(0);
         LED_SCL(1);
       }
     }
  }
  LED_LAT(1);
  LED_LAT(0);
}
void open_line(byte x)
{
  switch (x)
  {
    case 0 :open_line0;
            break;
    case 1 :open_line1;
            break;
    case 2 :open_line2;
            break;
    case 3 :open_line3;
            break;
    case 4 :open_line4;
            break;
    case 5 :open_line5;
            break;
    case 6 :open_line6;
            break;
    case 7 :open_line7;
            break;
    default: close_all_line;
            break;
  }
}

void SetMirrorMode(byte mirrorX, byte mirrorY)
{
    if (mirrorX) {
        xmask = 0xff;
        xoff = 8;
    } else {
        xmask = 0;
        xoff = 0;
    }
    if (mirrorY) {
        ymask = 0xff;
        yoff = 8;
    } else {
        ymask = 0;
        yoff = 0;
    }
}

void SetPixel(byte x, byte y, byte color[])
{
    byte page = 1 - pageIndex;
    x = ((char)x ^ xmask) + xoff;
    y = ((char)y ^ ymask) + yoff;
    dots[page][y][x][2] = color[0];
    dots[page][y][x][1] = color[1];
    dots[page][y][x][0] = color[2];
}

void SetPixelCurPage(byte x, byte y, byte color[])
{
    byte page = pageIndex;
    x = ((char)x ^ xmask) + xoff;
    y = ((char)y ^ ymask) + yoff;
    dots[page][y][x][2] = color[0];
    dots[page][y][x][1] = color[1];
    dots[page][y][x][0] = color[2];
}

void ClearPixel(byte x, byte y)
{
    byte page = 1 - pageIndex;
    x = ((char)x ^ xmask) + xoff;
    y = ((char)y ^ ymask) + yoff;
    dots[page][y][x][2] = 0;
    dots[page][y][x][1] = 0;
    dots[page][y][x][0] = 0;
}

void InitLED()
{
    _IO_Init();           //Init IO
    _LED_Init();          //Init LED Hardware
    _TC2_Init();          //Init Timer/Count2
    BlankLED(1);
}

/****************************************************
the timer2 operate functions zone
****************************************************/
ISR(TIMER2_OVF_vect)          //Timer2  Service
{
  cli();
  TCNT2 = 0x64;      //flash a led matrix frequency is 100.3Hz,period is 9.97ms
  //TCNT2 = 0x63;      //flash a led matrix frequency is 99.66Hz,period is 10.034ms
    close_all_line;
    run(line);
    open_line(line);
    line = (line + 1) & 0x7;
    sei();
}

#else

//=============================================
//MBI5168
#define SH_DIR_OE    DDRC
#define SH_DIR_SDI   DDRC
#define SH_DIR_CLK   DDRC
#define SH_DIR_LE    DDRC

#define SH_BIT_OE    0x08
#define SH_BIT_SDI   0x01
#define SH_BIT_CLK   0x02
#define SH_BIT_LE    0x04

#define SH_PORT_OE   PORTC
#define SH_PORT_SDI  PORTC
#define SH_PORT_CLK  PORTC
#define SH_PORT_LE   PORTC
//============================================
#define clk_rising  {SH_PORT_CLK&=~SH_BIT_CLK;SH_PORT_CLK|=SH_BIT_CLK;}
#define le_high     {SH_PORT_LE|=SH_BIT_LE;}
#define le_low      {SH_PORT_LE&=~SH_BIT_LE;}
#define enable_oe   {SH_PORT_OE&=~SH_BIT_OE;}
#define disable_oe  {SH_PORT_OE|=SH_BIT_OE;}

#define shift_data_1     {SH_PORT_SDI|=SH_BIT_SDI;}
#define shift_data_0     {SH_PORT_SDI&=~SH_BIT_SDI;}
//============================================
#define open_line0	{PORTB=0x04;}
#define open_line1	{PORTB=0x02;}
#define open_line2	{PORTB=0x01;}
#define open_line3	{PORTD=0x80;}
#define open_line4	{PORTD=0x40;}
#define open_line5	{PORTD=0x20;}
#define open_line6	{PORTD=0x10;}
#define open_line7	{PORTD=0x08;}
#define close_all_line	{PORTD&=~0xf8;PORTB&=~0x07;}
//============================================

#define CheckRequest (g8Flag1&0x01)
#define SetRequest (g8Flag1|=0x01)
#define ClrRequest (g8Flag1&=~0x01)

//==============================================
#define waitingcmd 0x00
#define processing  0x01
#define checking  0x02

#define showPrefabnicatel 0x01
#define showChar 0x02
#define showColor 0x03

/*
const unsigned char GammaTab[16]=
{0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0xE7};
*/
#define GAMMA 0xE7

//========================================================
unsigned char dots[2][3][8][4];

//=============================================================
unsigned char line,level;
unsigned char State=0;

void SetMirrorMode(byte mirrorX, byte mirrorY)
{
    if (mirrorX) {
        xmask = 0xff;
        xoff = 8;
    } else {
        xmask = 0;
        xoff = 0;
    }
    if (mirrorY) {
        ymask = 0;
        yoff = 0;
    } else {
        ymask = 0xff;
        yoff = 8;
    }
}

void SetPixel(byte x, byte y, byte color[])
{
    x = (char)(x ^ xmask) + xoff;
    y = (char)(y ^ ymask) + yoff;
    byte page = 1 - pageIndex;
    if ((x & 1)) {
        x >>= 1;
        dots[page][0][y][x] = (color[1] >> 4) | (dots[page][0][y][x] & 0xF0);
        dots[page][1][y][x] = (color[0] >> 4) | (dots[page][1][y][x] & 0xF0);
        dots[page][2][y][x] = (color[2] >> 4) | (dots[page][2][y][x] & 0xF0);
    } else {
        x >>= 1;
        dots[page][0][y][x] = (color[1] & 0xF0) | (dots[page][0][y][x] & 0x0F);
        dots[page][1][y][x] = (color[0] & 0xF0) | (dots[page][1][y][x] & 0x0F);
        dots[page][2][y][x] = (color[2] & 0xF0) | (dots[page][2][y][x] & 0x0F);
    }
}

void SetPixelCurPage(byte x, byte y, byte color[])
{
    x = (char)(x ^ xmask) + xoff;
    y = (char)(y ^ ymask) + yoff;
    byte page = pageIndex;
    if ((x & 1)) {
        x >>= 1;
        dots[page][0][y][x] = (color[1] >> 4) | (dots[page][0][y][x] & 0xF0);
        dots[page][1][y][x] = (color[0] >> 4) | (dots[page][1][y][x] & 0xF0);
        dots[page][2][y][x] = (color[2] >> 4) | (dots[page][2][y][x] & 0xF0);
    } else {
        x >>= 1;
        dots[page][0][y][x] = (color[1] & 0xF0) | (dots[page][0][y][x] & 0x0F);
        dots[page][1][y][x] = (color[0] & 0xF0) | (dots[page][1][y][x] & 0x0F);
        dots[page][2][y][x] = (color[2] & 0xF0) | (dots[page][2][y][x] & 0x0F);
    }
}

void ClearPixel(byte x, byte y)
{
    x = (char)(x ^ xmask) + xoff;
    y = (char)(y ^ ymask) + yoff;
    byte page = 1 - pageIndex;
    if ((x & 1)) {
        x >>= 1;
        dots[page][0][y][x] &= 0xF0;
        dots[page][1][y][x] &= 0xF0;
        dots[page][2][y][x] &= 0xF0;
    } else {
        x >>= 1;
        dots[page][0][y][x] &= 0x0F;
        dots[page][1][y][x] &= 0x0F;
        dots[page][2][y][x] &= 0x0F;
    }
}

#ifndef RAINBOW_REMOTE
void init_timer2(void)
{
  TCCR2A |= (1 << WGM21) | (1 << WGM20);
  TCCR2B |= (1<<CS22);   // by clk/64
  TCCR2B &= ~((1<<CS21) | (1<<CS20));   // by clk/64
  TCCR2B &= ~((1<<WGM21) | (1<<WGM20));   // Use normal mode
  ASSR |= (0<<AS2);       // Use internal clock - external clock not used in Arduino
  TIMSK2 |= (1<<TOIE2) | (0<<OCIE2B);   //Timer2 Overflow Interrupt Enable
#ifdef GAMMA
  TCNT2 = GAMMA;
#else
  TCNT2 = GammaTab[0];
#endif
  sei();
}

//==============================================================
void shift_1_bit(unsigned char LS)  //shift 1 bit of  1 Byte color data into Shift register by clock
{
  if(LS)
  {
    shift_data_1;
  }
  else
  {
    shift_data_0;
  }
  clk_rising;
}

//==============================================================
void shift_24_bit(unsigned char line,unsigned char level)   // display one line by the color level in buff
{
  unsigned char color=0,row=0;
  unsigned char data0=0,data1=0;
  le_high;

  for(color=0;color<3;color++)//GBR
  {
    for(row=0;row<4;row++)
    {
      data1=dots[pageIndex][color][line][row]&0x0f;
      data0=dots[pageIndex][color][line][row]>>4;

      //gray scale,0x0f aways light
      shift_1_bit(data0 > level ? 1 : 0);
      shift_1_bit(data1 > level ? 1 : 0);
    }
  }
  le_low;
}


//==============================================================
void open_line(unsigned char line)     // open the scaning line
{
  switch(line)
  {
  case 0: open_line0; break;
  case 1: open_line1; break;
  case 2: open_line2; break;
  case 3: open_line3; break;
  case 4: open_line4; break;
  case 5: open_line5; break;
  case 6: open_line6; break;
  case 7: open_line7; break;
  }
}

//==============================================================
void flash_next_line(unsigned char line,unsigned char level) // scan one line
{
  disable_oe;
  close_all_line;
  open_line(line);
  shift_24_bit(line,level);
  enable_oe;
}

ISR(TIMER2_OVF_vect)          //Timer2  Service
{
#ifdef GAMMA
    TCNT2 = GAMMA;
#else
    TCNT2 = GammaTab[level];    // Reset a  scanning time by gamma value table
#endif

    flash_next_line(line,level);  // scan the next line in LED matrix level by level.
    // when have scaned all LEC the back to line 0 and add the level
    if(++line>7) {
        line=0;
        /*
        level++;
        if(level>15) level=0;
        */
        level = (level + 1) & 0xf;
    }
}

void InitLED()
{
    DDRD=0xff;
    DDRC=0xff;
    DDRB=0xff;
    PORTD=0;
    PORTB=0;
    BlankLED(1);
    init_timer2();  // initial the timer for scanning the LED matrix
}
#endif

#endif

#ifndef RAINBOW_REMOTE
void Draw()
{
    pageIndex = 1 - pageIndex;
}
#endif

void BlankLED(char allPages)
{
    if (allPages) {
        memset(dots, 0, sizeof(dots));
    } else {
        memset(&dots[1 - pageIndex], 0, sizeof(dots[0]));
    }
}
