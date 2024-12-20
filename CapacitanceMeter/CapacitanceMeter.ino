/* 
Capacitance meter
sourced from https://www.circuitbasics.com/how-to-make-an-arduino-capacitance-meter/#
*/

const int OUT_PIN = A2;
const int IN_PIN = A0;
const float IN_STRAY_CAP_TO_GND = 24.48;
const float IN_CAP_TO_GND  = IN_STRAY_CAP_TO_GND;
const float R_PULLUP = 34.8;  
const int MAX_ADC_VALUE = 1023;
const int NUM_SAMPLES = 64;  // Number of samples for averaging

unsigned long previousMillis = 0;
int sampleCount = 0;
float totalCapacitance = 0;

void setup() {
  pinMode(OUT_PIN, OUTPUT);
  pinMode(IN_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (sampleCount < NUM_SAMPLES) {
    // Introduce a delay between each measurement
    delay(10);

    pinMode(IN_PIN, INPUT);
    digitalWrite(OUT_PIN, HIGH);
    int val = analogRead(IN_PIN);
    digitalWrite(OUT_PIN, LOW);

    float capacitance;
    if (val < 1000) {
      pinMode(IN_PIN, OUTPUT);
      capacitance = (float)val * IN_CAP_TO_GND / (float)(MAX_ADC_VALUE - val);

      Serial.print(F("Capacitance Value = "));
      Serial.print(capacitance, 3);
      Serial.print(F(" pF ("));
      Serial.print(val);
      Serial.println(F(") "));
    } else {
      pinMode(IN_PIN, OUTPUT);
      delay(1);
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
      delay(dischargeTime);   
      pinMode(OUT_PIN, OUTPUT);  
      digitalWrite(OUT_PIN, LOW);
      digitalWrite(IN_PIN, LOW);

      capacitance = -(float)t / R_PULLUP / log(1.0 - (float)val / (float)MAX_ADC_VALUE);

      Serial.print(F("Capacitance Value = "));
      if (capacitance > 1000.0) {
        Serial.print(capacitance / 1000.0, 2);
        Serial.print(F(" uF"));
      } else {
        Serial.print(capacitance, 2);
        Serial.print(F(" nF"));
      }

      Serial.print(F(" ("));
      Serial.print(digVal == 1 ? F("Normal") : F("HighVal"));
      Serial.print(F(", t= "));
      Serial.print(t);
      Serial.print(F(" us, ADC= "));
      Serial.print(val);
      Serial.println(F(")"));
    }

    totalCapacitance += capacitance;
    sampleCount++;
  } else {
    // Calculate the average capacitance
    float averageCapacitance = totalCapacitance / (float)NUM_SAMPLES;

    // Output capacitance for debugging purposes
    Serial.print(F("Average Capacitance = "));
    Serial.print(averageCapacitance, 3);
    Serial.println(F(" pF"));

    // Reset for the next cycle
    sampleCount = 0;
    totalCapacitance = 0;
  }

  while (millis() % 1000 != 0);    
}
