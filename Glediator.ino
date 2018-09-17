// Glediator example with OctoWS2811, by mortonkopf
//
// https://forum.pjrc.com/threads/33012-Gladiator-with-OctoWS2811-working-example

// You can also use Jinx to record Glediator format data to a SD card.
// To play the data from your SD card, use this modified program:
// https://forum.pjrc.com/threads/46229&viewfull=1#post153927
/*
#include <OctoWS2811.h>

const int ledsPerStrip = 360;
const unsigned int matrix_width = 47;
const unsigned int matrix_height = 60;
#define WIDTH 47
#define HEIGHT 60
int x = 0;
int y = 0;

DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];

const int config = WS2811_GRB | WS2811_800kHz;
OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

void setup() {
  Serial.begin(1000000);
  leds.begin();
  leds.show();
}

int serialGlediator() {
  while (!Serial.available()) {}
  return Serial.read();
}


unsigned int xy(unsigned int x, unsigned int y) {
  if ((x & 1) == 0) {
    // even numbered rows (0, 2, 4...) are left to right
    return (x * matrix_height) + y;
  } else {
    // odd numbered rows (1, 3, 5...) are right to left
    return (x * matrix_height) + matrix_height - 1 - y;
  }
}
//HL_BL
void loop() {
  byte r,g,b;
  int i;
  int x, y;
  while (serialGlediator() != 1) {}
  for (x = 0; x < WIDTH * 2; x++) {
    for (y = 0; y < HEIGHT; y++) {
      if (x & 1) {
        while (!Serial.available()) ;
        Serial.read();
        while (!Serial.available()) ;
        Serial.read();
        while (!Serial.available()) ;
        Serial.read();
        continue;
      }
      b = serialGlediator();
      r = serialGlediator();
      g = serialGlediator();
      leds.setPixel(xy(x/2,y), Color(r,g,b));
  }
}
  */
/*
  for (i = 0; i < WIDTH * HEIGHT * 2; i++) {
    if (i & 1 && i != WIDTH) { continue; }
      b = serialGlediator();
      r = serialGlediator();
      g = serialGlediator();
      leds.setPixel(xy(x,y), Color(r,g,b));
      x = (y & 1) ? (x - 1) : (x + 1);
      y = (x == -1 || x == matrix_width) ? (y + 1) : y;
      x = (x == -1) ? (x + 1) : x;
      x = (x == matrix_width) ? (x - 1) : x;

      if (y == matrix_height && x == 47) {
        y = 0;
        x = 0;
      }
  }

  leds.show();
}
*/
/* Helper functions */
// Create a 24 bit color value from R,G,B
unsigned int Color(byte r, byte g, byte b)
{
  return (((unsigned int)b << 16) | ((unsigned int)r << 8) | (unsigned int)g);
}
