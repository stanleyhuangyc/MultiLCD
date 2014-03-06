#include <SPI.h>
#include <Wire.h>
#include <MultiLCD.h>

LCD_ILI9341 lcd; /* for 2.2" SPI TFT module */

void setup() {
  // set up LCD display
  lcd.begin();
  lcd.setFont(FONT_SIZE_MEDIUM);
  lcd.print("Analog input:");
  lcd.setCursor(160, 0);
  lcd.print("Button:");
  lcd.setFont(FONT_SIZE_XLARGE);
  // set up pin mode
  pinMode(A3, INPUT);
  pinMode(8, INPUT);
  // set up serial port baudrate
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  int value = analogRead(A3);
  lcd.setCursor(0, 3);
  lcd.printInt(value, 4);

  int btn = digitalRead(8);
  lcd.setCursor(160, 3);
  lcd.print(btn ? "ON " : "OFF");

  // output analog value via serial UART
  Serial.println(value);
  
  delay(50);
}
