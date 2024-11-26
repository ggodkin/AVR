#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

hd44780_I2Cexp lcd; // declare lcd object: auto locate & auto config expander chip
const int OUT_PIN = A2;
const int IN_PIN = A0;
const float IN_STRAY_CAP_TO_GND = 24.48;
const float IN_CAP_TO_GND = IN_STRAY_CAP_TO_GND;
const float R_PULLUP = 34.8;  
const int MAX_ADC_VALUE = 1023;

// LCD geometry
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

void setup(){
  int status;

  pinMode(OUT_PIN, OUTPUT);
  pinMode(IN_PIN, OUTPUT);
	status = lcd.begin(LCD_COLS, LCD_ROWS);
	if(status) // non zero status means it was unsuccesful
	{
		// hd44780 has a fatalError() routine that blinks an led if possible
		// begin() failed so blink error code using the onboard LED if possible
		hd44780::fatalError(status); // does not return
	}
}

void loop(){
    pinMode(IN_PIN, INPUT);
    digitalWrite(OUT_PIN, HIGH);
    int val = analogRead(IN_PIN);
    digitalWrite(OUT_PIN, LOW);

    if (val < 1000){
      pinMode(IN_PIN, OUTPUT);

      float capacitance = (float)val * IN_CAP_TO_GND / (float)(MAX_ADC_VALUE - val);

      lcd.setCursor(0,0);
      lcd.print(F("Capacitance = "));
      lcd.setCursor(0,1);
      lcd.print(capacitance, 3);
      lcd.print(F("pF "));
      lcd.print(val);
      lcd.print("mS");
    }
    
    else{
      pinMode(IN_PIN, OUTPUT);
      delay(1);
      pinMode(OUT_PIN, INPUT_PULLUP);
      unsigned long u1 = micros();
      unsigned long t;
      int digVal;

      do{
        digVal = digitalRead(OUT_PIN);
        unsigned long u2 = micros();
        t = u2 > u1 ? u2 - u1 : u1 - u2;
      } 
      
      while ((digVal < 1) && (t < 400000L));

      pinMode(OUT_PIN, INPUT);  
      val = analogRead(OUT_PIN);
      digitalWrite(IN_PIN, HIGH);
      int dischargeTime = (int)(t / 1000L) * 5;
      delay(dischargeTime);   
      pinMode(OUT_PIN, OUTPUT);  
      digitalWrite(OUT_PIN, LOW);
      digitalWrite(IN_PIN, LOW);

      float capacitance = -(float)t / R_PULLUP / log(1.0 - (float)val / (float)MAX_ADC_VALUE);

      lcd.setCursor(0,0);
      lcd.print(F("Capacitance = "));
      if (capacitance > 1000.0){
        lcd.setCursor(0,1);
        lcd.print(capacitance / 1000.0, 2);
        lcd.print(F("uF "));
        lcd.print(val);
        lcd.print("mS");
      }
        
      else{
        lcd.setCursor(0,1);
        lcd.print(capacitance, 2);
        lcd.print(F("nF "));
        lcd.print(val);
        lcd.print("mS");
      }
  }
    while (millis() % 1000 != 0);
}