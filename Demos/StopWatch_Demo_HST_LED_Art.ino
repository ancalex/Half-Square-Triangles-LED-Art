#include "FastLED.h"
#define COLOR_ORDER GRB
#define CHIPSET WS2811

#define NUM_LEDS      128
#define BRIGHTNESS    255
CRGB leds[NUM_LEDS];

const uint8_t m_leds[128] = {
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

//numbers
int zero[] = {8, 9, 24, 25, 32, 33, 48, 49};

int two[] = {0, 1, 25, 32, 56, 57};

boolean numbers[10][16] = {
		{0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0},
		{0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1},
		{1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1},
		{1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1},
		{1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1},
		{1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1},
		{1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1},
		{1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0},
		{1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1},
		{1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1}
};

int second = 0;
int secondx10 = 0;
int minute = 0 ;
int minutex10 = 0;


void number_display (int position, int number, CRGB number_color) {
	for (int i = 0; i < 16; i+= 2) {
		if (numbers[number][i] == 1) {
			leds[m_leds[position + i/2*8]] = number_color;
			//leds[m_leds[position + i/2*8]] = CRGB(128,128,0);
		}
		else {
			leds[m_leds[position + i/2*8]] = CRGB(0,0,0);
		}
		if (numbers[number][i + 1] == 1) {
			leds[m_leds[position + 1 + i/2*8]] = number_color;
			//leds[m_leds[position + 1 + i/2*8]] = CRGB(128,128,0);
		}
		else {
			leds[m_leds[position + 1 + i/2*8]] = CRGB(0,0,0);
		}
	}
}

void setup()
{
	Serial.begin(115200);
	FastLED.addLeds<CHIPSET, 5, COLOR_ORDER>(leds, 0, 32).setCorrection( TypicalLEDStrip );
	FastLED.addLeds<CHIPSET, 4, COLOR_ORDER>(leds, 32, 32).setCorrection( TypicalLEDStrip );
	FastLED.addLeds<CHIPSET, 3, COLOR_ORDER>(leds, 64, 32).setCorrection( TypicalLEDStrip );
	FastLED.addLeds<CHIPSET, 2, COLOR_ORDER>(leds, 96, 32).setCorrection( TypicalLEDStrip );
	FastLED.setBrightness(BRIGHTNESS);
	fill_solid( leds, NUM_LEDS, CRGB(0,0,0));
	FastLED.show();
}

void loop()
{
	if (second == 10) {
		second = 0;
		secondx10++;
	}
	number_display(5, second, CRGB::BlueViolet);
	if (secondx10 == 6) {
		secondx10 = 0;
		minute++;
	}
	number_display(1, secondx10, CRGB::BlueViolet);
	if (minute == 10) {
		minute = 0;
		minutex10++;
	}
	number_display(69, minute, CRGB::CRGB::Gold);
	if (minutex10 == 10) {
		minutex10 = 0;
	}
	number_display(65, minutex10, CRGB::CRGB::Gold);
	FastLED.show();
	delay(1000);
	second++;
}

