#define FASTLED_INTERRUPT_RETRY_COUNT 0
#include "FastLED.h"
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS 128
CRGB leds[NUM_LEDS];
int BRIGHTNESS = 80;

const byte m_leds[128] = {
		111, 112, 79, 80, 47, 48, 15, 16,
		110, 113, 78, 81, 46, 49, 14, 17,
		109, 77, 114, 45, 82, 13, 50, 18,
		108, 76, 115, 44, 83, 12, 51, 19,
		75, 107, 43, 116, 11, 84, 20, 52,
		74, 106, 42, 117, 10, 85, 21, 53,
		73, 41, 105, 9, 118, 22, 86, 54,
		72, 40, 104, 8, 119, 23, 87, 55,
		39, 71, 7, 103, 24, 120, 56, 88,
		38, 70, 6, 102, 25, 121, 57, 89,
		37, 5, 69, 26, 101, 58, 122, 90,
		36, 4, 68, 27, 100, 59, 123, 91,
		3, 35, 28, 67, 60, 99, 92, 124,
		2, 34, 29, 66, 61, 98, 93, 125,
		1, 30, 33, 62, 65, 94, 97, 126,
		0, 31, 32, 63, 64, 95, 96, 127
};

const byte q1_leds[32] = {
		0, 1, 2, 3,
		8, 9, 10, 11,
		16, 17, 18, 19,
		24, 25, 26, 27,
		32, 33, 34, 35,
		40, 41, 42, 43,
		48, 49, 50, 51,
		56, 57, 58, 59
};

void setup()
{
	Serial.begin(115200);
	// tell FastLED about the LED strip configuration
	FastLED.addLeds<LED_TYPE, 5>(leds, 0, 32).setCorrection(TypicalSMD5050);
	FastLED.addLeds<LED_TYPE, 4>(leds, 32, 32).setCorrection(TypicalSMD5050);
	FastLED.addLeds<LED_TYPE, 3>(leds, 64, 32).setCorrection(TypicalSMD5050);
	FastLED.addLeds<LED_TYPE, 2>(leds, 96, 32).setCorrection(TypicalSMD5050);
	FastLED.setBrightness(BRIGHTNESS);
	Serial.println("FastLed Setup done");
	fill_solid( leds, NUM_LEDS, CRGB(0,0,0));
}

// The loop function is called in an endless loop
void loop()
{
	random16_add_entropy( random());
	byte random_leds_number = random8(8, 30);
	for (int i = 0; i < random_leds_number; i++) {
		byte random_led = random8(0, 31);
		byte color_r = random8(32, 255);
		byte color_g = random8(32, 255);
		byte color_b = random8(32, 255);
		leds[m_leds[q1_leds[random_led]]] = CRGB(color_g,color_r,color_b);
		//vertical symmetry
		byte v_offset = 7 - 2*(q1_leds[random_led]%8);
		byte v_led = q1_leds[random_led] + v_offset;
		leds[m_leds[v_led]] = CRGB(color_g,color_r,color_b);
		//horizontal symmetry
		byte h_led = 8*(15 - int(q1_leds[random_led]/8)) + q1_leds[random_led]%8 ;
		leds[m_leds[h_led]] = CRGB(color_g,color_r,color_b);
		//vertical + horizontal symmetry
		byte vh_led = v_offset + h_led;
		leds[m_leds[vh_led]] = CRGB(color_g,color_r,color_b);
	}
	FastLED.delay(2000);
	for (int i = 255; i > 40; i-=20) {
		for ( int j = 0; j < 128; j++) {
			leds[j].nscale8(i);
			FastLED.show();
		}
	}
	fill_solid( leds, NUM_LEDS, CRGB(0,0,0));
	FastLED.show();
}


