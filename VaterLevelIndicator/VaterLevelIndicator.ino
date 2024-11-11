/*
Capacitance-based Liquid Level Indicator
sourced from https://www.circuitbasics.com/how-to-make-an-arduino-capacitance-meter/#
*/

const int OUT_PIN = A2;
const int IN_PIN = A0;
const float IN_STRAY_CAP_TO_GND = 24.48;
const float IN_CAP_TO_GND = IN_STRAY_CAP_TO_GND;
const float R_PULLUP = 34.8;  
const int MAX_ADC_VALUE = 1023;
const int NUM_SAMPLES = 64;
const unsigned long MEASUREMENT_INTERVAL = 10; // Time between measurements in milliseconds

const int ledPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11}; // Pins connected to the LED bar graph
const int NUM_LEDS = sizeof(ledPins) / sizeof(ledPins[0]);

float minLevel = 100.0;  // Minimum capacitance level
float maxLevel = 1000.0; // Maximum capacitance level

unsigned long previousMillis = 0;
int sampleCount = 0;
long totalVal = 0;
unsigned long totalTime = 0;

void setup() {
  pinMode(OUT_PIN, OUTPUT);
  pinMode(IN_PIN, OUTPUT);
  Serial.begin(9600);
  
  // Set LED pins as outputs
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= MEASUREMENT_INTERVAL) {
    previousMillis = currentMillis;

    // Take a sample
    pinMode(IN_PIN, INPUT);
    digitalWrite(OUT_PIN, HIGH);
    int val = analogRead(IN_PIN);
    digitalWrite(OUT_PIN, LOW);

    if (val < 1000) {
      pinMode(IN_PIN, OUTPUT);
      totalVal += val;
    } else {
      pinMode(IN_PIN, OUTPUT);
      pinMode(OUT_PIN, INPUT_PULLUP);
      unsigned long u1 = micros();
      unsigned long t;
      int digVal;

      do {
        digVal = digitalRead(OUT_PIN);
        unsigned long u2 = micros();
        t = u2 > u1 ? u2 - u1 : u1 - u2;
      } while ((digVal < 1) && (t < 400000L));

      pinMode(OUT_PIN, INPUT);  
      val = analogRead(OUT_PIN);
      digitalWrite(IN_PIN, HIGH);
      int dischargeTime = (int)(t / 1000L) * 5;
      totalTime += t;
    }

    sampleCount++;

    // If we've taken the required number of samples, calculate the average and capacitance
    if (sampleCount >= NUM_SAMPLES) {
      float averageVal = totalVal / (float)NUM_SAMPLES;
      float averageTime = totalTime / (float)NUM_SAMPLES;

      float capacitance;
      if (averageVal < 1000) {
        capacitance = averageVal * IN_CAP_TO_GND / (float)(MAX_ADC_VALUE - averageVal);
        Serial.print(F("Capacitance Value = "));
        Serial.print(capacitance, 3);
        Serial.print(F(" pF ("));
        Serial.print(averageVal);
        Serial.println(F(") "));
      } else {
        capacitance = -(float)averageTime / R_PULLUP / log(1.0 - (float)averageVal / (float)MAX_ADC_VALUE);
        Serial.print(F("Capacitance Value = "));
        if (capacitance > 1000.0) {
          Serial.print(capacitance / 1000.0, 2);
          Serial.print(F(" uF"));
        } else {
          Serial.print(capacitance, 2);
          Serial.print(F(" nF"));
        }
        Serial.print(F(" (t= "));
        Serial.print(averageTime);
        Serial.print(F(" us, ADC= "));
        Serial.print(averageVal);
        Serial.println(F(")"));
      }

      // Calculate the liquid level as a percentage
      float level = map(capacitance, minLevel, maxLevel, 0, NUM_LEDS);
      level = constrain(level, 0, NUM_LEDS);

      // Update the LED bar graph
      for (int i = 0; i < NUM_LEDS; i++) {
        digitalWrite(ledPins[i], i < level ? HIGH : LOW);
      }

      // Reset for the next cycle
      sampleCount = 0;
      totalVal = 0;
      totalTime = 0;
    }
  }
}
