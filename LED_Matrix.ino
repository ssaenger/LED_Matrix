//------------------------------------------------------------------------------
/*
 * Created by Shawn Saenger with adaption from other contributers
 */

#include <Audio.h>
#include <OctoWS2811.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include "Color.h"
#include "GPIO.h"
//#include <SerialFlash.h>

// Normally the buttons are used to change the LED matrix state and colors
// Setting this define to 1 will allow you to use the buttons to try out
// new level values (sensitivity of LEDs).
#define TRY_NEW_LEVEL_VALUES 0

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

// Some default levels
#define MAX_LEVEL      0.30f  // 1.0 = max, lower is more "sensitive"
#define DYNAMIC_RANGE  40.0f  // Smaller number = harder to overcome init thresh
#define LINEAR_BLEND   0.5f   // useful range is 0 to 0.7

#define MAX_LEVELS_PROFILE 4
//---

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

typedef struct _levels {
  float max_level;
  float dynamic_range;
  float linear_blend;
} levels;

levels levelsArray[MAX_LEVELS_PROFILE];


// Holds the RGB values filled up by a call to makeColor()
// Don't change this value unless you modify makeColor()
color_t rainbowColors[COLOR_GRADIENT];

// This array holds the volume level (0 to 1.0) for each
// vertical pixel to turn on.  Computed in setup() using
// the 3 parameters above.
float thresholdVertical[HEIGHT];
float thresholdHorizontal[WIDTH];

// Spectrum arrays
uint32_t* freqBinWidth;
uint32_t* freqBinHeight;

// Index of the fixedColors array
uint32_t fixedOnColor;
uint32_t fixedOffColor;
uint32_t onColorInd;
uint32_t offColorInd;
uint8_t isDynamic_onColor;
uint8_t isRainbow_onColor;
uint8_t isDynamic_offColor;
uint8_t isRainbow_offColor;


#define NUM_STATES 10 // Make sure to update this if adding/removing states
enum sm_states { fft_bot_st,   // shoot up from bottom
                 fft_top_st,   // shoot down from top
                 fft_btb_st,   // shoot from Both Top and Bottom
                 fft_btbF_st,  // shoot from both top and bottom, fliggped
                 fft_mid_st,   // shoot from middle
                 fft_side_st,  // shoot from sides
                 fft_sideF_st, // shoot from sides, flipped
                 rainbow_st,   // display the rainbow
                 plaz_st,      // display neat math rainbow
                 idle_st,      // pretty, non-bright ambient look
                 off_st        // NUM_STATES doesn't count this.
};
uint8_t LED_state;
uint8_t LED_statePrev;
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Prototypes
void GPIO_init();
void GPIO_debounce();
void Coor_plotLEDs(uint8_t LED_state, uint32_t* Bin4Width, uint32_t* Bin4Height);
void Coor_mixedColors(uint8_t LED_state);
void color_HSLtoRGB(uint32_t  saturation,
                    uint32_t  lightness,
                    uint32_t* rainbowColors);
uint32_t* spectrum_getBin1();
uint32_t* spectrum_getBin2();
uint32_t* spectrum_getBin3();
//uint32_t map_xy(uint32_t x, uint32_t y);
//void init_gridState(uint8_t LED_state);
void updateLedState(buttonVal_t buttonPress, uint8_t wasHeld);
void updateColor(buttonVal_t colorButton);
void computeLevels(float maxLevel, float dynamicRange, float linearBlend);
inline uint8_t fastCosineCalc(uint16_t preWrapVal);
void Coor_off();
void Coor_testAll(uint8_t repeat);

//------------------------------------------------------------------------------

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
  onColor = fixedOnColors;
  offColor = fixedOffColors;
  onColorInd = 1;
  offColorInd = 0;
  isDynamic_onColor = 0;
  isRainbow_onColor = 0;
  isDynamic_offColor = 0;
  isRainbow_offColor = 0;
  fixedOnColor = NAVY;
  fixedOffColor = BLACK;

  // Compute the vertical threshold
  levelsArray[0] = {0.06, 32.00, 0.30}; // Very quiet music
  levelsArray[1] = {0.10, 30.00, 0.60}; // Medium music
  levelsArray[2] = {0.12, 24.00, 0.20}; // Medium music
  levelsArray[3] = {0.28, 40.00, 0.50}; // LOUD music
  computeLevels(levelsArray[0].max_level,
                levelsArray[0].dynamic_range,
                levelsArray[0].linear_blend);

  freqBinWidth  = spectrum_getBin0();
  freqBinHeight = spectrum_getBin3();

  if (!(freqBinWidth && freqBinHeight)) {
    Serial.println("one of the bins is too long");
    exit(1);
  }
  LED_state = fft_mid_st;
  LED_statePrev = fft_bot_st;
  leds.begin();
  Coor_testAll(0);

}

void loop()
{
  buttonVal_t buttonPress;
  uint8_t wasHeld = 0;

  Coor_plotLEDs(LED_state, freqBinWidth, freqBinHeight); // Only returns once button is pressed

  // Debounce any buttons. This line of code will be run
  // once a button is pressed.
  buttonPress = GPIO_debounce(&wasHeld);
  updateLedState(buttonPress, wasHeld);

}

// Depending on the button press and if it was held or not, this will update
// the variable LED_state
void updateLedState(buttonVal_t buttonPress, uint8_t wasHeld)
{
  static uint8_t i, x = 0;

  if (wasHeld) {
    // For now, treat any button press as same action
    LED_statePrev = LED_state;
    LED_state = off_st;
    return;
  }

#if TRY_NEW_LEVEL_VALUES == 0

  switch (buttonPress) {
    case 0:
      break;

    case BUTTON_RIGHT: // On Color
    case BUTTON_LEFT: // Off Color
      updateColor(buttonPress);
      break;

    case BUTTON_DOWN:
      if (LED_state == off_st) {
        LED_state = LED_statePrev;
        LED_statePrev = off_st;
      }
      LED_state = (LED_state == 0) ? (NUM_STATES - 1) : (LED_state - 1);
      break;

    case BUTTON_UP:
      if (LED_state == off_st) {
        LED_state = LED_statePrev;
        LED_statePrev = off_st;
      }
      LED_state = (LED_state + 1) % NUM_STATES;
      break;

    default:
      if (buttonPress == (BUTTON_LEFT | BUTTON_DOWN)) {
        i = (i + 1) % MAX_LEVELS_PROFILE;
        computeLevels(levelsArray[i].max_level,
                      levelsArray[i].dynamic_range,
                      levelsArray[i].linear_blend);
      }
      else if (buttonPress == (BUTTON_RIGHT | BUTTON_DOWN)) {
        switch (x) {
          case 0:
            freqBinWidth = spectrum_getBin0();
            x = 1;
            break;

          case 1:
            freqBinWidth = spectrum_getBin1();
            x = 2;
            break;

          case 2:
            freqBinWidth = spectrum_getBin2();
            x = 0;
            break;

          default:
            x = 0;
            break;
        }
        if (!freqBinWidth) {
          // Oops, a freq bin is too long
          exit(1);
        }
      }
      break;
    }

#else

  switch (buttonPress) {
    case 0:
      break;

    case BUTTON_RIGHT:
      max_level = (max_level < 0.03) ? 0.90 : max_level - 0.02;
      break;
    case BUTTON_LEFT:
      max_level = (max_level > 0.89) ? 0.02 : max_level + 0.02;
      break;

    case BUTTON_DOWN:
      dynamic_range = (dynamic_range < 3) ? DYNAMIC_RANGE + 10 : dynamic_range - 2;
      break;
    case BUTTON_UP:
      dynamic_range = (dynamic_range > DYNAMIC_RANGE + 10) ? 4 : dynamic_range + 2;
      break;

    default:
      if (buttonPress == (BUTTON_LEFT | BUTTON_DOWN)) {
        linear_blend = (linear_blend < 0.11) ? 0.7 : linear_blend - 0.05;
      }
      else if (buttonPress == (BUTTON_LEFT | BUTTON_UP)) {
        linear_blend = (linear_blend > 0.69) ? 0.10 : linear_blend + 0.05;
      }
      break;
  }
        computeLevels(max_level, dynamic_range, linear_blend);

#endif // TRY_NEW_LEVEL_VALUES == 0


}


void updateColor(buttonVal_t colorButton)
{
  if (colorButton == BUTTON_LEFT) {
    // update OnColor
    if (isRainbow_onColor) {
      if (isDynamic_onColor) {
        isDynamic_onColor = 0;
        isRainbow_onColor = 0;
      }
      else {
        isDynamic_onColor = 1;
      }
    }
    else {
      onColorInd = (onColorInd + 1) % FIXED_COLOR_ON_NUM;
      fixedOnColor = fixedOnColors[onColorInd];
      if (fixedOnColor == BLACK) {
        isRainbow_onColor = 1;
        isDynamic_onColor = 0;
      }
    }

  }
  else {
    // update offColor
    if (isRainbow_offColor) {
      if (isDynamic_offColor) {
        isDynamic_offColor = 0;
        isRainbow_offColor = 0;
      }
      else {
        isDynamic_offColor = 1;
      }
    }
    else {
      offColorInd = (offColorInd + 1) % FIXED_COLOR_ON_NUM;
      fixedOffColor = fixedOffColors[offColorInd];
      if (fixedOffColor == BLACK) {
        isRainbow_offColor = 1;
        isDynamic_offColor = 0;
      }
    }
  }
}

// Computes the vertical and horizontal levels. Call during setup and when
// wanting to change the levels after button presses.
// maxLevel  = 1.0 is max, lower is more "sensitive"
// dynamicRange = Smaller number means harder to overcome init thresh
// linearBlend = useful range is 0 to 0.7
void computeLevels(float maxLevel, float dynamicRange, float linearBlend)
{
  uint8_t x, y;
  float n, logLevel, linearLevel;

#if TRY_NEW_LEVEL_VALUES == 1
  Serial.print("maxLevel = ");
  Serial.println(maxLevel);
  Serial.print("dynamicRange = ");
  Serial.println(dynamicRange);
  Serial.print("linearBlend = ");
  Serial.println(linearBlend);
  Serial.println("---------------------");
#endif // TRY_NEW_LEVEL_VALUES == 1

// Compute vertical values (for when LED_state is showing bars going up-down)
  for (y = 0; y < HEIGHT; y++) {
    n = (float)y / (float)(HEIGHT - 1);
    logLevel = pow10f(n * -1.0 * (dynamicRange / 20.0));
    linearLevel = 1.0 - n;
    linearLevel = linearLevel * linearBlend;
    logLevel = logLevel * (1.0 - linearBlend);
    thresholdVertical[HEIGHT - y - 1] = (logLevel + linearLevel) * maxLevel;
    //Serial.print(HEIGHT - y - 1);
    //Serial.print(": ");
    //Serial.print(thresholdVertical[HEIGHT - y - 1], 5);
    //Serial.print("\n");
  }
  // Compute horizontal values (for when LED_state is showing bars going
  // side-to-side)
  for (x = 0; x < WIDTH; x++) {
    n = (float)x / (float)(WIDTH - 1);
    logLevel = pow10f(n * -1.0 * (dynamicRange / 20.0));
    linearLevel = 1.0 - n;
    linearLevel = linearLevel * linearBlend;
    logLevel = logLevel * (1.0 - linearBlend);
    thresholdHorizontal[WIDTH - x - 1] = (logLevel + linearLevel) * maxLevel;
    //Serial.print(WIDTH - x - 1);
    //Serial.print(": ");
    //Serial.print(thresholdHorizontal[WIDTH - x - 1], 5);
    //Serial.print("\n");
  }
}
