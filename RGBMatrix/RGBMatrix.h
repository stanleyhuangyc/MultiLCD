#ifndef RGBLED_h
#define RGBLED_h

#ifndef RAINBOW
//#define RAINBOW
#endif

void InitLED();
void SetPixel(byte x, byte y, byte color[]);
void SetPixelCurPage(byte x, byte y, byte color[]);
void ClearPixel(byte x, byte y);
void Draw();
void BlankLED(char allPages);
void SetMirrorMode(byte mirrorX, byte mirrorY);

#endif
