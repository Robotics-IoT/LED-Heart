#include <Adafruit_MLX90614.h>
#include <CircularBuffer.h>
#include <FastLED.h>

#define NUM_LEDS        12
#define DATA_PIN        5
#define PERIOD          5000
#define ANIMATION_TIME  PERIOD

#define MAX_BRIGHTNESS  255
#define MIN_BRIGHTNESS  0

#define SAMPLE_SIZE     100
#define SAMPLE_TIME     10 /* 10ms */

#define MIN_TEMP        31.0
#define MAX_TEMP        36.0

#define RUN_FOR_N_MILLISECONDS(N) \
  for(uint32_t start = millis(); (millis()- start) < N; )


CRGB leds[NUM_LEDS];
CircularBuffer<float, SAMPLE_SIZE> buffer;

static volatile uint32_t now;

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup() {

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(MAX_BRIGHTNESS);
  FastLED.clear(true);

  Wire.setClock(400000L);
  delay(10);
  mlx.begin();

  /* Human skin emissivity */
  mlx.writeEmissivity(0.95);

  now = millis();
}

void loop() {

  float moving_avg = 0.0;

  if((millis() - now) > SAMPLE_TIME) {
    buffer.push(mlx.readObjectTempC());
    now = millis();
  }
  if(buffer.isFull()) {
    using index_t = decltype(buffer)::index_t;

    for (index_t i = 0; i < buffer.size(); i++) {
			moving_avg += buffer[i] / (float)buffer.size();
		}

    if (moving_avg >= MIN_TEMP && moving_avg <= MAX_TEMP) {
      heart_effect();
      heartbeat();
      buffer.clear();
      moving_avg = 0.0;
      now = millis();
    }
  }
}

void heart_effect() {

  int left_led;
  int right_led;
  FastLED.setBrightness(MAX_BRIGHTNESS);

  for (left_led = 0, right_led = 11; left_led <= 6 && right_led >= 6; left_led++, right_led--) {
    leds[left_led] = CRGB::Red;
    leds[right_led] = CRGB::Red;
    FastLED.show();
    delay(90);
  }
  delay(1000);
  FastLED.clear(true);
  FastLED.show();
}

void heartbeat() {

  int curr_steps = 0;
  int step_incr = 8;

  FastLED.clear(true);
  FastLED.show();
  RUN_FOR_N_MILLISECONDS(ANIMATION_TIME) {

    for (uint8_t i = 0; i < 12; i++) {
      leds[i] = CRGB::Red;
    }
    FastLED.setBrightness(curr_steps);
    FastLED.show();
    delay(45);
    curr_steps += step_incr;

    if (curr_steps > MAX_BRIGHTNESS) {
      curr_steps = MAX_BRIGHTNESS;
      step_incr *= -1;
    }
    else if ( curr_steps < MIN_BRIGHTNESS) {
      curr_steps = MIN_BRIGHTNESS;
      step_incr *= -1;
    }
  }
  FastLED.clear(true); 
  FastLED.show(); 
}
