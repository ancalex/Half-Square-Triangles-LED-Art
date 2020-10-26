#define FASTLED_INTERRUPT_RETRY_COUNT 0
#include "FastLED.h"
#include "HSTModels.h"
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS 128
CRGB leds[NUM_LEDS];
int BRIGHTNESS = 160;

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

const byte hst_cell_offset[] = {0, 1, 9, 17, 25, 24, 16, 8, 0, 1, 9, 17, 25, 24, 16, 8};
const byte hst_cell_origin[] = {0, 2, 4, 6, 32, 34, 36, 38, 64, 66, 68, 70, 96, 98, 100, 102};
byte hst_cell_position[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //array for intermediary hst positions
byte animation_steps[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //how many animation steps between startand end position
byte orig;
byte pos;
byte modelNumber = 0;

byte blendRate = 10;  // How fast to blend.  Higher is slower.  [milliseconds]
byte k = 0;

CRGB colorStart = CRGB(255,255,255); // starting color
CRGB colorTarget = CRGB(0,0,0); // target color
//CRGB colorStart = CRGB(202,243,32); // starting color
//CRGB colorTarget = CRGB(0,0,0); // target color
//CRGB colorStart = CRGB(0,0,32); // starting color
//CRGB colorTarget = CRGB(0,255,255); // target color
//CRGB colorStart = CRGB(0,32,32); // starting color
//CRGB colorTarget = CRGB(255,255,0); // target color
//CRGB colorStart = CRGB(33,229,101); // starting color***
//CRGB colorTarget = CRGB(17,13,55); // target color
//CRGB colorStart = CRGB(203,121,184); // starting color
//CRGB colorTarget = CRGB(4,80,114); // target color
CRGB colorCurrent1 = colorStart;
CRGB colorCurrent2 = colorTarget;

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

	for (int i = 0; i < 16; i++) {      // homing :) Display 16 HST in zero position
		int origin = hst_cell_origin[i];
		for (int j = 0; j < 8; j++) {
			int offset = hst_cell_offset[j];
			if (j < 4) {
				leds[m_leds[origin + offset]] = colorStart;
			}
			else {
				leds[m_leds[origin + offset]] = colorTarget;
			}
		}
		FastLED.show();
		delay(100);
	}
	delay(1000);
}

// The loop function is called in an endless loop
void loop()
{
	for (int m = 0; m < 16; m++) {
		int step = pgm_read_byte(&HSTM[modelNumber][m]) - hst_cell_position[m]; //how many steps from start to end position
 		if (step >= 0 ) {
			animation_steps[m] = 2 * step; // 0-1, 0-2, 0-3, 1-2, 1-3, 2-3 positions
		}
		else {
			animation_steps[m] = 2 * (4 + step); //3-0, 3-1, 3-2, 2-1, 2-0, 1-0
		}
		Serial.print(animation_steps[m]);Serial.print("-");
	}
	Serial.println();
	for (int i = 0; i < 8; i++) { //8 steps of animation one 360 degrees rotation
		while (colorCurrent1 != colorTarget) {
			EVERY_N_MILLISECONDS(blendRate) {
				colorCurrent1 = blend(colorStart, colorTarget, k); //color of one end of the triangle
				colorCurrent2 = blend(colorTarget, colorStart, k); //other end of the triangle
				for (int j = 0; j < 16; j++) { //animate all hst
					if (animation_steps[j] > 0) { // but only if there are any remaining steps for a hst
					orig = hst_cell_origin[j];
					leds[m_leds[orig + hst_cell_offset[i + (hst_cell_position[j] * 2)]]] = colorCurrent1; //color of one end of the triangle
					leds[m_leds[orig + hst_cell_offset[i + 4 + (hst_cell_position[j] * 2)]]] = colorCurrent2; //other end of the triangle
					//
					}
				}
				k+=5; //animation speed, is working only with 1 and 5 !!!
				FastLED.show();  // update the display
			}
		}
		for (int n = 0; n < 16; n++) { //one step less after an iteration
		if (animation_steps[n] > 0) {
			animation_steps[n]--;
		}
		}
		k = 0;
		colorCurrent1 = colorStart;
		colorCurrent2 = colorTarget;
	}
	for (int n = 0; n < 16; n++) {
	hst_cell_position[n] = pgm_read_byte(&HSTM[modelNumber][n]); // save the actual model positions
	}
	modelNumber++;
	if (modelNumber == 73) {modelNumber = 0;}
	FastLED.delay(2000);
}





