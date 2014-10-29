#include <SPI.h>
#include <MultiLCD.h>

LCD_ILI9341 lcd; /* for 2.2" SPI TFT module */

#define ANALOG_PIN A3
#define BUTTON_PIN 8

const PROGMEM uint8_t tick[16 *16 / 8] =
{0x00,0x80,0xC0,0xE0,0xC0,0x80,0x00,0x80,0xC0,0xE0,0xF0,0xF8,0xFC,0x78,0x30,0x00,0x00,0x01,0x03,0x07,0x0F,0x1F,0x1F,0x1F,0x0F,0x07,0x03,0x01,0x00,0x00,0x00,0x00};

const PROGMEM uint8_t cross[16 *16 / 8] =
{0x00,0x0C,0x1C,0x3C,0x78,0xF0,0xE0,0xC0,0xE0,0xF0,0x78,0x3C,0x1C,0x0C,0x00,0x00,0x00,0x30,0x38,0x3C,0x1E,0x0F,0x07,0x03,0x07,0x0F,0x1E,0x3C,0x38,0x30,0x00,0x00};

void setup() {
  // set up LCD display
  lcd.begin();
  lcd.setFontSize(FONT_SIZE_MEDIUM);
  lcd.print("Analog input:");
  lcd.setCursor(160, 0);
  lcd.print("Button:");
  lcd.setFontSize(FONT_SIZE_XLARGE);
  // set up pin mode
  pinMode(ANALOG_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);
  // set up serial port baudrate
  Serial.begin(115200);
}

void loop() {
  // display analog input value
  int value = analogRead(ANALOG_PIN);
  lcd.setColor(RGB16_YELLOW);
  lcd.setCursor(0, 3);
  lcd.printInt(value, 4);

  // draw tick (when button is pressed) or cross (when button is not pressed)
  int btn = digitalRead(BUTTON_PIN);
  lcd.setXY(160, 20);
  if (!btn) {
    lcd.setColor(RGB16_GREEN);
    lcd.draw(tick, 16, 16, 2);
    // reset button state
    digitalWrite(BUTTON_PIN, HIGH);
  } else {
    lcd.setColor(RGB16_RED);
    lcd.draw(cross, 16, 16, 2);
  }

  // output analog value via serial UART
  Serial.println(value);

  delay(50);
}
