
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

SoftwareSerial bluetoothModulu(10, 11); 
 double x;
 double y;


// with the arduino pin number it is connected to
LiquidCrystal lcd(12, 9, 5, 4, 3, 2);

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  Serial.begin(9600);
  bluetoothModulu.begin(9600); // Default communication rate of the Bluetooth module
  
  
}

void loop() {
  //lcd.clear();
  
  if(bluetoothModulu.available()>0){ // Checks whether data is comming from the serial port
    x = bluetoothModulu.read();
    y = bluetoothModulu.read();
    Serial.println("x=");
    Serial.println(x);     
    Serial.println("y=");
    Serial.println(y);
    if(x>128)
     {
      x=x-256;
     }  
       if(y>128)
     {
      y=y-256;
     } 
    }
    
  lcd.setCursor(0, 0);
  lcd.print("x=");
  lcd.print(x);
  lcd.setCursor(0, 1);
  lcd.print("y=");
  lcd.print(y);
  lcd.display();
  delay(70);
}
