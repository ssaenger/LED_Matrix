//------------------------------------------------------------------------------
/*
 * Created by Shawn Saenger with adaption from other contributers
 */

#include <OctoWS2811.h>
#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include "Color.h"
//#include <SerialFlash.h>

//------------------------------------------------------------------------------
// FFT definitions. Audio comes from computer through USB
AudioInputUSB            usb1;
AudioOutputI2S           i2s1;           // Needed
AudioMixer4              mixer1;
AudioAnalyzeFFT1024      fft;
AudioConnection          patchCord1(usb1, 0, mixer1, 0);
AudioConnection          patchCord2(usb1, 1, mixer1, 1);
AudioConnection          patchCord3(mixer1, fft);

//------------------------------------------------------------------------------
// OctoWS2811 definitions
#define LEDS_PER_PIN   360    // My LED Matrix is divided up into 8 diff sectors
#define MAX_LEVEL      0.20f  // 1.0 = max, lower is more "sensitive"
#define DYNAMIC_RANGE  30.0f  // total range to display, in decibels
#define LINEAR_BLEND   0.7f   // useful range is 0 to 0.7
DMAMEM uint32_t displayMemory[LEDS_PER_PIN*6];
uint32_t drawingMemory[LEDS_PER_PIN*6];
const uint32_t config = WS2811_GRB | WS2811_800kHz;
OctoWS2811 leds(LEDS_PER_PIN, displayMemory, drawingMemory, config);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Size of my matrix
#define WIDTH  47
#define HEIGHT 60
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Globals

// Holds the RGB values filled up by a call to makeColor()
// Don't change this value unless you modify makeColor()
uint32_t rainbowColors[COLOR_GRADIENT];

// This array holds the volume level (0 to 1.0) for each
// vertical pixel to turn on.  Computed in setup() using
// the 3 parameters above.
float thresholdVertical[HEIGHT];

// Spectrum arrays
uint32_t* freqBins1;
uint32_t* freqBins2;

enum sm_states { fft_bot_st,  // shoot up from bottom
                 fft_mid_st,  // shoot from middle
                 fft_top_st,  // shoot down from top
                 rainbow_st,  // display the rainbow
                 plaz_st,     // display neat math rainbow
                 glediator_st // Use the glediator software on PC
};
sm_states state;
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Prototypes
void color_HSLtoRGB(uint32_t saturation,
                    uint32_t lightness,
                    uint32_t* rainbowColors);
uint32_t* spectrum_getBin1();
uint32_t* spectrum_getBin2();
uint32_t map_xy(uint32_t x, uint32_t y);
void computeVerticalLevels();
inline uint8_t fastCosineCalc(uint16_t preWrapVal);
//------------------------------------------------------------------------------

void setup()
{
  uint8_t x;
  delay(2000);

  AudioMemory(20); // The audio library needs memory to begin.
  fft.windowFunction(AudioWindowHanning1024);
  // Fill out rainbowColors[]
  color_HSLtoRGB(SATURATION, LIGHTNESS, rainbowColors);

  // Compute the vertical threshold
  computeVerticalLevels();

  freqBins1 = spectrum_getBin1();
  freqBins2 = spectrum_getBin2();

  leds.begin();

  state = fft_bot_st;
  // turn on LED for bottom row. We'll skip updating this row for fft_bot_st
  for (x = 0; x < WIDTH; x++) {
    leds.setPixel(map_xy(x, 0), rainbowColors[x+x+0]);
  }
}

void loop()
{
  static uint32_t frameCount = 25500;
  static float    level;
  static uint32_t color;
  static uint16_t t, t2, t3;
  static uint8_t  x, y;
  static uint8_t  freqBin;
  static uint8_t  r, g, b;

  switch (state) {
    case fft_bot_st:
      if (fft.available()) {
        freqBin = 0;
        for (x = 0; x < WIDTH; x++) {
          // get the volume for each horizontal pixel position
          level = fft.read(freqBin, freqBin + freqBins2[x] - 1);

          for (y = 1; y < HEIGHT; y++) {
            // for each vertical pixel, check if above the threshold
            // and turn the LED on or off
            if (level >= thresholdVertical[y]) {
            //leds.setPixel(xy(x, y), myColor);
              leds.setPixel(map_xy(x, y), rainbowColors[x+x+y]);
            } else {
              leds.setPixel(map_xy(x, y), 0x000000);
            }
          }
          freqBin = freqBin + freqBins2[x];
        }
      }
      break;
    case fft_mid_st:
      break;
    case fft_top_st:
      break;
    case rainbow_st:
      break;
    case plaz_st:
      frameCount++ ;
      t  = fastCosineCalc((42 * frameCount)/100); // time displacement
      t2 = fastCosineCalc((35 * frameCount)/100); // fiddle with these
      t3 = fastCosineCalc((38 * frameCount)/100); // to change looks

      for (x = 0; x < WIDTH; x++) {
        for (y = 0; x < HEIGHT ; y++) {
          //Calculate 3 seperate plasma waves, one for each color channel
          r = fastCosineCalc(((x << 3) +
                  (t >> 1) + fastCosineCalc((t2 + (y << 3)))));
          g = fastCosineCalc(((y << 3) +
                  t + fastCosineCalc(((t3 >> 2) + (x << 3)))));
          b = fastCosineCalc(((y << 3) +
                  t2 + fastCosineCalc((t + x + (g >> 2)))));
          //uncomment the following to enable gamma correction
          //r=pgm_read_byte_near(exp_gamma+r);
          //g=pgm_read_byte_near(exp_gamma+g);
          //b=pgm_read_byte_near(exp_gamma+b);
          leds.setPixel(map_xy(x,y), ((r << 16) | (g << 8) | b));
        }
      }
      break;
    case glediator_st:
      break;
    default:
      break;
  }

  // update the LED matrix
  leds.show();
}

// Selects the correct number from x and y coordinate values
// note: may be quicker with ternary op
uint32_t map_xy(uint32_t x, uint32_t y)
{
  if ((x & 1) == 0) {
    // even numbered columns (0, 2, 4...) left to right
    return (x * HEIGHT) + y;
  }
  else {
    // odd numbered columns (1, 3, 5...) left to right
    return (x * HEIGHT) + HEIGHT - 1 - y;
  }
}

// Run once from setup, the compute the vertical levels
void computeVerticalLevels()
{
  uint8_t y;
  float n, logLevel, linearLevel;

  for (y = 0; y < HEIGHT; y++) {
    n = (float)y / (float)(HEIGHT - 1);
    logLevel = pow10f(n * -1.0 * (DYNAMIC_RANGE / 20.0));
    linearLevel = 1.0 - n;
    linearLevel = linearLevel * LINEAR_BLEND;
    logLevel = logLevel * (1.0 - LINEAR_BLEND);
    thresholdVertical[HEIGHT - y - 1] = (logLevel + linearLevel) * MAX_LEVEL;
    Serial.print(HEIGHT - y - 1);
    Serial.print(": ");
    Serial.print(thresholdVertical[HEIGHT - y - 1], 5);
    Serial.print("\n");
  }
}
