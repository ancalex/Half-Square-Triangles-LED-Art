/*

  FastLED Fire 2018 by Stefan Petrick

  The visual effect highly depends on the framerate.
  In the Youtube video it runs at arround 70 fps.
  https://www.youtube.com/watch?v=SWMu-a9pbyk

  The heatmap movement is independent from the framerate.
  The actual scaling operation is not.
  Check out the code for further comments about the interesting parts

*/

#include "FastLED.h"
#define COLOR_ORDER GRB
#define CHIPSET WS2811

uint8_t Width  = 8;
uint8_t Height = 16;
uint8_t CentreX =  (Width / 2) - 1;
uint8_t CentreY = (Height / 2) - 1;

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

// control parameters for the noise array

uint32_t x;
uint32_t y;
uint32_t z;
uint32_t scale_x;
uint32_t scale_y;

// storage for the noise data
// adjust the size to suit your setup
uint8_t noise[8][16];

// heatmap data with the size matrix width * height
uint8_t heat[128];

// the color palette
CRGBPalette16 Pal;

// check the Serial Monitor for fps rate
void show_fps() {
  EVERY_N_MILLIS(100) {
    Serial.println(LEDS.getFPS());
  }
}

// this finds the right index within a serpentine matrix
uint16_t XY( uint8_t x, uint8_t y) {
  uint16_t i;
  i = m_leds[y * 8 + x];
  return i;
}


// here we go
void Fire2018() {

  // get one noise value out of a moving noise space
  uint16_t ctrl1 = inoise16(11 * millis(), 0, 0);
  // get another one
  uint16_t ctrl2 = inoise16(13 * millis(), 100000, 100000);
  // average of both to get a more unpredictable curve
  uint16_t  ctrl = ((ctrl1 + ctrl2) / 2);

  // this factor defines the general speed of the heatmap movement
  // high value = high speed
  //uint8_t speed = 27;
  uint8_t speed = 32;

  // here we define the impact of the wind
  // high factor = a lot of movement to the sides
  x = 3 * ctrl * speed;

  // this is the speed of the upstream itself
  // high factor = fast movement
  y = 15 * millis() * speed;

  // just for ever changing patterns we move through z as well
  z = 3 * millis() * speed ;

  // ...and dynamically scale the complete heatmap for some changes in the
  // size of the heatspots.
  // The speed of change is influenced by the factors in the calculation of ctrl1 & 2 above.
  // The divisor sets the impact of the size-scaling.
  scale_x = ctrl1 / 2;
  scale_y = ctrl2 / 2;

  // Calculate the noise array based on the control parameters.
  uint8_t layer = 0;
  for (uint8_t i = 0; i < Width; i++) {
    uint32_t ioffset = scale_x * (i - CentreX);
    for (uint8_t j = 0; j < Height; j++) {
      uint32_t joffset = scale_y * (j - CentreY);
      uint16_t data = ((inoise16(x + ioffset, y + joffset, z)) + 1);
      noise[i][j] = data >> 8;
    }
  }


  // Draw the first (lowest) line - seed the fire.
  // It could be random pixels or anything else as well.
  for (uint8_t x = 0; x < Width; x++) {
    // draw
    leds[XY(x, Height-1)] = ColorFromPalette( Pal, noise[x][0]);
    // and fill the lowest line of the heatmap, too
    heat[XY(x, Height-1)] = noise[x][0];
  }

  // Copy the heatmap one line up for the scrolling.
  for (uint8_t y = 0; y < Height - 1; y++) {
    for (uint8_t x = 0; x < Width; x++) {
      heat[XY(x, y)] = heat[XY(x, y + 1)];
    }
  }

  // Scale the heatmap values down based on the independent scrolling noise array.
  for (uint8_t y = 0; y < Height - 1; y++) {
    for (uint8_t x = 0; x < Width; x++) {

      // get data from the calculated noise field
      uint8_t dim = noise[x][y];

      // This number is critical
      // If it´s to low (like 1.1) the fire dosn´t go up far enough.
      // If it´s to high (like 3) the fire goes up too high.
      // It depends on the framerate which number is best.
      // If the number is not right you loose the uplifting fire clouds
      // which seperate themself while rising up.
      dim = dim / 1.4;

      dim = 255 - dim;

      // here happens the scaling of the heatmap
      heat[XY(x, y)] = scale8(heat[XY(x, y)] , dim);
    }
  }

  // Now just map the colors based on the heatmap.
  for (uint8_t y = 0; y < Height - 1; y++) {
    for (uint8_t x = 0; x < Width; x++) {
      leds[XY(x, y)] = ColorFromPalette( Pal, heat[XY(x, y)]);
    }
  }

  // Done. Bring it on!
  FastLED.show();

  // I hate this delay but with 8 bit scaling there is no way arround.
  // If the framerate gets too high the frame by frame scaling doesn´s work anymore.
  // Basically it does but it´s impossible to see then...

  // If you change the framerate here you need to adjust the
  // y speed and the dim divisor, too.
  delay(10);

}


void setup() {
  Serial.begin(115200);
  FastLED.addLeds<CHIPSET, 5, COLOR_ORDER>(leds, 0, 32).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<CHIPSET, 4, COLOR_ORDER>(leds, 32, 32).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<CHIPSET, 3, COLOR_ORDER>(leds, 64, 32).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<CHIPSET, 2, COLOR_ORDER>(leds, 96, 32).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setDither(DISABLE_DITHER);
  Pal = HeatColors_p;
}

void loop() {
  Fire2018();
  //show_fps();
}

