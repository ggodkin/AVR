#include <FastLED.h>

#define LED_PIN1       3
#define LED_PIN2       4 // Pin for the second LED string
#define NUM_LEDS       215
#define MAX_BRIGHTNESS 128
#define PIR_PIN_BEGIN  7
#define PIR_PIN_END    5
#define FADE_DURATION  25000 // 30 seconds in milliseconds
#define DELAY_BETWEEN_LEDS 15 // Time delay in milliseconds between lighting up each LED

CRGB leds1[NUM_LEDS];
// CRGB leds2[NUM_LEDS]; - both strings should be the same

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN1, GRB>(leds1, NUM_LEDS);
  FastLED.addLeds<WS2812B, LED_PIN2, GRB>(leds1, NUM_LEDS);
  pinMode(PIR_PIN_BEGIN, INPUT);
  pinMode(PIR_PIN_END, INPUT);
  randomSeed(millis() % 1000); // Seed the random number generator using mod of milliseconds
}

void loop() {
  if (digitalRead(PIR_PIN_BEGIN) == HIGH) {
    uint8_t hue = random(0, 255);
    activateLEDs(0, hue);
    fadeOutLEDs();
  } else if (digitalRead(PIR_PIN_END) == HIGH) {
    uint8_t hue = random(0, 255);
    activateLEDs(NUM_LEDS - 1, hue);
    fadeOutLEDs();
  }
}

void activateLEDs(int startIndex, uint8_t hue) {
  FastLED.setBrightness(MAX_BRIGHTNESS); // Ensure starting brightness is consistent

  if (startIndex == 0) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds1[i] = CHSV((hue + i) % 255, 255, 255); // Change hue as LEDs turn on
      // leds2[i] = CHSV((hue + i) % 255, 255, 255); // Same for the second string
      FastLED.show();
      delay(DELAY_BETWEEN_LEDS);
    }
  } else {
    for (int i = NUM_LEDS - 1; i >= 0; i--) {
      leds1[i] = CHSV((hue + NUM_LEDS - 1 - i) % 255, 255, 255); // Change hue as LEDs turn on
      // leds2[i] = CHSV((hue + NUM_LEDS - 1 - i) % 255, 255, 255); // Same for the second string
      FastLED.show();
      delay(DELAY_BETWEEN_LEDS);
    }
  }
}

void fadeOutLEDs() {
  unsigned long startTime = millis();
  while (millis() - startTime < FADE_DURATION) {
    uint8_t brightness = map(millis() - startTime, 0, FADE_DURATION, MAX_BRIGHTNESS, 0);
    FastLED.setBrightness(brightness);
    FastLED.show();
    delay(30); // Adjust delay for smoother fading
  }
  FastLED.clear(); // Ensure LEDs are off at the end
  FastLED.show();
}
