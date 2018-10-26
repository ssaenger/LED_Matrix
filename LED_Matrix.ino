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
#include "GPIO.h"
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
#define MAX_LEVEL      0.30f  // 1.0 = max, lower is more "sensitive"
#define DYNAMIC_RANGE  40.0f  // Smaller number = harder to overcome init thresh
#define LINEAR_BLEND   0.5f   // useful range is 0 to 0.7
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

volatile byte isr_flag;

// Color pointer
color_t* curColor;

color_t* onColor;
color_t* offColor;



// Holds the RGB values filled up by a call to makeColor()
// Don't change this value unless you modify makeColor()
color_t rainbowColors[COLOR_GRADIENT];

// This array holds the volume level (0 to 1.0) for each
// vertical pixel to turn on.  Computed in setup() using
// the 3 parameters above.
float thresholdVertical[HEIGHT];
float thresholdHorizontal[WIDTH];

// Spectrum arrays
uint32_t* freqBins1;
uint32_t* freqBins2;
uint32_t* freqBins3;


#define NUM_STATES 9 // Make sure to update this if adding/removing states
enum sm_states { fft_bot_st,   // shoot up from bottom
                 fft_top_st,   // shoot down from top
                 fft_btb_st,   // shoot from Both Top and Bottom
                 fft_btbF_st,  // shoot from both top and bottom, fliggped
                 fft_mid_st,   // shoot from middle
                 fft_side_st,  // shoot from sides
                 fft_sideF_st, // shoot from sides, flipped
                 rainbow_st,   // display the rainbow
                 plaz_st,      // display neat math rainbow
                 //glediator_st, // Use the glediator software on PC
                 off_st
};
uint8_t LED_state;
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Prototypes
void GPIO_init();
void GPIO_debounce();
void color_HSLtoRGB(uint32_t  saturation,
                    uint32_t  lightness,
                    uint32_t* rainbowColors);
uint32_t* spectrum_getBin1();
uint32_t* spectrum_getBin2();
uint32_t* spectrum_getBin3();
//uint32_t map_xy(uint32_t x, uint32_t y);
void init_gridState(uint8_t LED_state);
void updateLedState(buttonVal_t buttonPress, uint8_t wasHeld);
void computeVerticalLevels();
inline uint8_t fastCosineCalc(uint16_t preWrapVal);
void off();
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Macros
#define map_xy_bot(x,y)   (x & 1) ? ((x * HEIGHT) + HEIGHT - 1 - y) : \
                                    ((x * HEIGHT) + y)

#define map_xy_top(x,y)   (x & 1) ? ((x * HEIGHT) + y) : \
                                    ((x * HEIGHT) + HEIGHT - 1 - y)

#define map_xy_top_F(x,y)   (x & 1) ? (((WIDTH - 1 - x) * HEIGHT) + y) : \
                                    (((WIDTH - x) * HEIGHT) - 1 - y)


// together
#define map_xy_midTop(x,y,o) (x & 1) ? ((x * HEIGHT) + HEIGHT - 1 - (y + o)) : \
                                       ((x * HEIGHT) + y + o)

#define map_xy_midBot(x,y,o) (x & 1) ? ((x * HEIGHT) + y + o) : \
                                       ((x * HEIGHT) + HEIGHT - 1 - (y + o))

// sides
#define map_xy_sidL(x,y)   (x & 1) ? ((x * HEIGHT) + HEIGHT - 1 - y) : \
                                     ((x * HEIGHT) + y)

#define map_xy_sidR(x,y)   (x & 1) ? (((WIDTH - x) * HEIGHT) - 1 - y) : \
                                     (((WIDTH - x) * HEIGHT) - HEIGHT + y)
// reversed top right side
#define map_xy_sidR_F(x,y)   (x & 1) ? (((WIDTH - 1 - x) * HEIGHT) + y) : \
                                       (((WIDTH - x) * HEIGHT) - 1 - y)


//------------------------------------------------------------------------------

void setup()
{
  delay(2000);
  isr_flag = 1;
  GPIO_init();
  AudioMemory(20); // The audio library needs memory to begin.
  fft.windowFunction(AudioWindowHanning1024);
  // Fill out rainbowColors[]
  color_HSLtoRGB(SATURATION, LIGHTNESS, rainbowColors);
  curColor = rainbowColors;
  onColor = fixedColors;
  offColor = fixedColors;

  // Compute the vertical threshold
  computeVerticalLevels();

  freqBins1 = spectrum_getBin1();
  freqBins2 = spectrum_getBin2();
  freqBins3 = spectrum_getBin3();

  LED_state = fft_side_st;
  leds.begin();
  //testAll();

}

void loop()
{
  static uint32_t frameCount = 0;
  static float    level;
  static uint16_t t, t2, t3;
  static int8_t   x, y;
  static uint16_t  freqBin;
  static uint8_t  r, g, b;
  buttonVal_t buttonPress;
  uint8_t wasHeld = 0;

  init_gridState(LED_state);
  switch (LED_state) {
    case fft_bot_st:
      while (isr_flag) {
        if (fft.available()) {
          freqBin = 0;
          for (x = 0; x < WIDTH; x++) {

            level = fft.read(freqBin, freqBin + freqBins2[x] - 1);
            for (y = 1; y < HEIGHT; y++) {

              if (level >= thresholdVertical[y]) {
                leds.setPixel(map_xy_bot(x, y), rainbowColors[x+x+y]);
              } else {
                leds.setPixel(map_xy_bot(x, y), BLACK);
              }
            }
            freqBin = freqBin + freqBins2[x];
          }
        }
        leds.show();
      }
      break;

      case fft_top_st:
        while (isr_flag) {
          if (fft.available()) {
            freqBin = 0;
            for (x = 0; x < WIDTH; x++) {
              level = fft.read(freqBin, freqBin + freqBins2[x] - 1);

              for (y = 1; y < HEIGHT; y++) {
                if (level >= thresholdVertical[y]) {
                  leds.setPixel(map_xy_top(x, y), rainbowColors[x+x+y]);
                } else {
                  leds.setPixel(map_xy_top(x, y), BLACK);
                }
              }
              freqBin = freqBin + freqBins2[x];
            }
          }
          leds.show();
        }
        break;

    case fft_btb_st:
      while (isr_flag) {
        if (fft.available()) {
          freqBin = 0;
          for (x = 0; x < WIDTH; x++) {
            level = fft.read(freqBin, freqBin + freqBins2[x] - 1);
            for (y = 1; y < HEIGHT / 2; y++) {
              if (level >= thresholdVertical[(y * 2) + 1]) {
                leds.setPixel(map_xy_bot(x, y), rainbowColors[x*5+y*2]);
                leds.setPixel(map_xy_top(x, y), rainbowColors[x*5+y*2]);
              } else {
                leds.setPixel(map_xy_bot(x, y), BLACK);
                leds.setPixel(map_xy_top(x, y), BLACK);
              }
            }
            freqBin = freqBin + freqBins2[x];
          }
        }
        leds.show();
      }
      break;

      case fft_btbF_st:
        while (isr_flag) {
          if (fft.available()) {
            freqBin = 0;
            for (x = 0; x < WIDTH; x++) {
              level = fft.read(freqBin, freqBin + freqBins2[x] - 1);
              for (y = 1; y < HEIGHT / 2; y++) {
                if (level >= thresholdVertical[(y * 2) + 1]) {
                  leds.setPixel(map_xy_bot(x, y), rainbowColors[x*5+y*2]);
                  leds.setPixel(map_xy_top_F(x, y), rainbowColors[x*5+y*2]);
                } else {
                  leds.setPixel(map_xy_bot(x, y), BLACK);
                  leds.setPixel(map_xy_top_F(x, y), BLACK);
                }
              }
              freqBin = freqBin + freqBins2[x];
            }
          }
          leds.show();
        }
        break;

    case fft_mid_st:
      while (isr_flag) {
        if (fft.available()) {
          freqBin = 0;
          frameCount++;

          for (x = 0; x < WIDTH; x++) {
            level = fft.read(freqBin, freqBin + freqBins2[x] - 1);
            for (y = 0; y < HEIGHT / 2 - 1; y++) {
              if (level >= thresholdVertical[(y * 2) + 1] +
                             (freqBins2[x] * thresholdVertical[0])) {
                leds.setPixel(map_xy_midBot(x, y, HEIGHT / 2 + 1),
                              curColor[(x * 4 + frameCount) % COLOR_GRADIENT]);
                leds.setPixel(map_xy_midTop(x, y, HEIGHT / 2 + 1),
                              curColor[(x * 4 + frameCount) % COLOR_GRADIENT]);
              }
              else {
                leds.setPixel(map_xy_midBot(x, y, HEIGHT / 2 + 1), BLACK);
                leds.setPixel(map_xy_midTop(x, y, HEIGHT / 2 + 1), BLACK);
              }
            }
            // Fixed, center LEDs
            leds.setPixel(map_xy_midBot(x, 0, HEIGHT / 2),
                          curColor[(x * 4 + frameCount) % COLOR_GRADIENT]);
            leds.setPixel(map_xy_midTop(x, 0, HEIGHT / 2),
                          curColor[(x * 4 + frameCount) % COLOR_GRADIENT]);
            freqBin = freqBin + freqBins2[x];
          }
          leds.show();
        }
      }
      break;

    case fft_side_st:
      while (isr_flag) {
        if (fft.available()) {
          freqBin = 0;
          for (y = 0; y < HEIGHT; y++) {
            level = fft.read(freqBin, freqBin + freqBins3[y] - 1);
              for (x = 1; x < WIDTH / 2; x++) {
              if (level >= thresholdHorizontal[(x * 2) + 1]) {
                leds.setPixel(map_xy_sidL(x, y), rainbowColors[y*6]);
                leds.setPixel(map_xy_sidR(x, y), rainbowColors[y*6]);
              } else {
                leds.setPixel(map_xy_sidL(x, y), BLACK);
                leds.setPixel(map_xy_sidR(x, y), BLACK);
              }
            }
            freqBin = freqBin + freqBins3[y];
          }
        }
        leds.show();
      }
      break;

    case fft_sideF_st:
      while (isr_flag) {
        if (fft.available()) {
          freqBin = 0;
          for (y = 0; y < HEIGHT; y++) {
            level = fft.read(freqBin, freqBin + freqBins3[y] - 1);
              for (x = 1; x < WIDTH / 2; x++) {
              if (level >= thresholdHorizontal[(x * 2) + 1]) {
                leds.setPixel(map_xy_sidL(x, y),   rainbowColors[y*6]);
                leds.setPixel(map_xy_sidR_F(x, y), rainbowColors[y*6]);
              } else {
                leds.setPixel(map_xy_sidL(x, y),   BLACK);
                leds.setPixel(map_xy_sidR_F(x, y), BLACK);
              }
            }
            freqBin = freqBin + freqBins3[y];
          }
        }
        leds.show();
      }
      break;

    case rainbow_st:
      while (isr_flag) {
        frameCount++;
        for (x = 1; x < WIDTH - 1; x++) {
          for (y = 1; y < HEIGHT - 1; y++) {
            leds.setPixel(map_xy_bot(x, y),
                          curColor[(x * 4 + y * 2 + frameCount) % COLOR_GRADIENT]);
          }
        }
        delay(50);
        leds.show();
      }
      break;

    case plaz_st:
      while(isr_flag) {
        frameCount++;
        t  = fastCosineCalc((30 * frameCount)/100); // time displacement
        t2 = fastCosineCalc((20 * frameCount)/100); // fiddle with these
        t3 = fastCosineCalc((45 * frameCount)/100); // to change looks

        for (x = 1; x < WIDTH - 1; x++) {
          for (y = 1; y < HEIGHT - 1; y++) {
            //Calculate 3 seperate plasma waves, one for each color channel
            r = fastCosineCalc(((x << 3) +
                    (t >> 1) + fastCosineCalc((t2 + (y << 3)))));
            g = fastCosineCalc(((y << 3) +
                    t + fastCosineCalc(((t3 >> 2) + (x << 3)))));
            b = fastCosineCalc(((y << 3) +
                    t2 + fastCosineCalc((t + x + (g >> 2)))));
            //uncomment the following to enable gamma correction
            r = exp_gamma[r];
            g = exp_gamma[g];
            b = exp_gamma[b];
            leds.setPixel(map_xy_bot(x,y), ((r << 16) | (g << 8) | b));
          }
        }
        leds.show();
      }
      break;
    //case glediator_st:
      //break;
      case off_st:
        while(isr_flag);
        break;
    default:
      break;
  }

  // Debounce any buttons. This line of code will be run
  // once a button is pressed.
  buttonPress = GPIO_debounce(&wasHeld);
  updateLedState(buttonPress, wasHeld);

}

void init_gridState(uint8_t LED_state)
{
  uint8_t   x, y;
  static uint8_t prev_state = off_st;

  if (LED_state == prev_state) {
    return;
  }
  prev_state = LED_state;

  off();
  switch (LED_state) {
    case fft_bot_st:
      for (x = 0; x < WIDTH; x++) {
        leds.setPixel(map_xy_bot(x, 0), rainbowColors[x+x+0]);
      }
      leds.show();
      break;

      case fft_top_st:
        for (x = 0; x < WIDTH; x++) {
          leds.setPixel(map_xy_top(x, 0), rainbowColors[x+x+0]);
        }
        leds.show();
        break;

    case fft_btb_st:
      for (x = 0; x < WIDTH; x++) {
        leds.setPixel(map_xy_bot(x, 0), rainbowColors[x*5]);
        leds.setPixel(map_xy_top(x, 0), rainbowColors[x*5]);
      }
      leds.show();
      break;

      case fft_btbF_st:
        for (x = 0; x < WIDTH; x++) {
          leds.setPixel(map_xy_bot(x, 0), rainbowColors[x*5]);
          leds.setPixel(map_xy_top_F(x, 0), rainbowColors[x*5]);
        }
        leds.show();
        break;

    case fft_mid_st:
      leds.show();
      break;

    case fft_side_st:
      for (y = 0; y < HEIGHT; y++) {
        leds.setPixel(map_xy_sidL(0, y), rainbowColors[y*6]);
        leds.setPixel(map_xy_sidR(0, y), rainbowColors[y*6]);
      }
      leds.show();
      break;

      case fft_sideF_st:
        for (y = 0; y < HEIGHT; y++) {
          leds.setPixel(map_xy_sidL(0, y),   rainbowColors[y*6]);
          leds.setPixel(map_xy_sidR_F(0, y), rainbowColors[y*6]);
        }
        leds.show();
        break;

    case rainbow_st:
      break;

    case plaz_st:
      break;

    //case glediator_st:
    //  break;

      case off_st:
        break;
    default:
      break;
  }
}

void updateLedState(buttonVal_t buttonPress, uint8_t wasHeld)
{

  if (wasHeld) {
    // For now, treat any button press as same action
    // TODO save last state and return to it when moving out of off_st
    LED_state = off_st;
    return;
  }

  switch (buttonPress) {
    case BUTTON_RIGHT:
    case BUTTON_LEFT:
      updateColor(buttonPress);
      break;
    case BUTTON_DOWN:
      LED_state = (LED_state == 0) ? (NUM_STATES - 1) : (LED_state - 1);
      break;
    case BUTTON_UP:
      LED_state = (LED_state + 1) % NUM_STATES;
      break;
    default:
      break;
  }
}

void updateColor(buttonVal_t colorButton)
{
  //static 
  if (colorButton == BUTTON_RIGHT) {
    // update OnColor
  }
  else {
    // update offColor

  }
}

// Run once from setup, the compute the vertical levels
void computeVerticalLevels()
{
  uint8_t x, y;
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
  for (x = 0; x < WIDTH; x++) {
    n = (float)x / (float)(WIDTH - 1);
    logLevel = pow10f(n * -1.0 * (DYNAMIC_RANGE / 20.0));
    linearLevel = 1.0 - n;
    linearLevel = linearLevel * LINEAR_BLEND;
    logLevel = logLevel * (1.0 - LINEAR_BLEND);
    thresholdHorizontal[WIDTH - x - 1] = (logLevel + linearLevel) * MAX_LEVEL;
    Serial.print(WIDTH - x - 1);
    Serial.print(": ");
    Serial.print(thresholdHorizontal[WIDTH - x - 1], 5);
    Serial.print("\n");
  }
}

void off() {
  uint8_t x, y;

  for (x = 0; x < WIDTH; x++) {
    for (y = 0; y < HEIGHT; y++) {
      leds.setPixel(map_xy_bot(x, y), BLACK);
    }
  }
  leds.show();
}


void testAll() {
  uint8_t x, y;
  for (x = 0; x < WIDTH; x++) {
    for (y = 0; y < HEIGHT ; y++) {
      leds.setPixel(map_xy_bot(x, y), DWHITE);
      leds.show();
      delay(5);
    }
    delay(10);
    for (y = 0; y < HEIGHT ; y++) {
      leds.setPixel(map_xy_bot(x, y), BLACK);
    }
    leds.show();
  }
}
