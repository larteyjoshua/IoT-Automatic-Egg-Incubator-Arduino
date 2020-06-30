#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
 lcd.begin(); // sixteen characters across - 2 lines
  lcd.backlight();
  // first character - 1st line
  
}

void loop()
{
 lcd.setCursor(0,0);
  lcd.print("hello");
  // 8th character - 2nd line 
   lcd.setCursor(0,1);
  lcd.print("-------");
}
