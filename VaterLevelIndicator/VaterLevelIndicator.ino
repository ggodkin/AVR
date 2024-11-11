/* 
Capacitance-based Liquid Level Indicator
sourced from https://www.circuitbasics.com/how-to-make-an-arduino-capacitance-meter/#
*/

const int OUT_PIN = A2;
const int IN_PIN = A0;
const float IN_STRAY_CAP_TO_GND = 24.48;
const float IN_CAP_TO_GND  = IN_STRAY_CAP_TO_GND;
const float R_PULLUP = 34.8;  
const int MAX_ADC_VALUE = 1023;

const int ledPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11}; // Pins connected to the LED bar graph
const int NUM_LEDS = sizeof(ledPins) / sizeof(ledPins[0]);

float minLevel = 12.8;  // Minimum capacitance level based on measurements
float maxLevel = 19.5;  // Maximum capacitance level based on measurements

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

    // Manual level calculation for debugging
    float level = (capacitance - minLevel) * NUM_LEDS / (maxLevel - minLevel);
    level = constrain(level, 0, NUM_LEDS);

    Serial.print(F("Manually Calculated Level = "));
    Serial.println(level);

    // Update the LED bar graph
    for (int i = 0; i < NUM_LEDS; i++) {
        digitalWrite(ledPins[i], i < level ? HIGH : LOW);
    }

    while (millis() % 1000 != 0);
}
