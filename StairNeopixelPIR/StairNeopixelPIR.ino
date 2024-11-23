#include <FastLED.h>

#define LED_PIN         3
#define NUM_LEDS        240
#define MAX_BRIGHTNESS  128
#define PIR_PIN_BEGIN   7
#define PIR_PIN_END     5
#define FADE_DURATION   30000 // 30 seconds in milliseconds
#define DELAY_BETWEEN_LEDS 100 // Time delay in milliseconds between lighting up each LED

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  pinMode(PIR_PIN_BEGIN, INPUT);
  pinMode(PIR_PIN_END, INPUT);
  randomSeed(analogRead(0)); // Seed the random number generator
}

void loop() {
  if (digitalRead(PIR_PIN_BEGIN) == HIGH) {
    activateLEDs(0);
    fadeOutLEDs();
  } else if (digitalRead(PIR_PIN_END) == HIGH) {
    activateLEDs(NUM_LEDS - 1);
    fadeOutLEDs();
  }
}

void activateLEDs(int startIndex) {
  uint8_t initialHue = random(0, 255); // Generate an initial random hue

  FastLED.setBrightness(MAX_BRIGHTNESS); // Ensure starting brightness is consistent

  if (startIndex == 0) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV((initialHue + i) % 255, 255, 255); // Change hue as LEDs turn on
      FastLED.show();
      delay(DELAY_BETWEEN_LEDS);
    }
  } else {
    for (int i = NUM_LEDS - 1; i >= 0; i--) {
      leds[i] = CHSV((initialHue + NUM_LEDS - 1 - i) % 255, 255, 255); // Change hue as LEDs turn on
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
