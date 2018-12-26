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


#define pick_color(x, f, c, r)   (r ? rainbowColors[((x) + f) % COLOR_GRADIENT] : c)
#define pick_color(x, f, c, r)  (r ? rainbowColors[((x) + f) % COLOR_GRADIENT] : c)

// The entire matrix has one color
void Coor_plotLEDs(uint8_t LED_state) {
  static uint32_t frameCount = 0;
  static uint32_t frameCountOff = 0;
  static float    level;
  static uint16_t t, t2, t3;
  static int8_t   x, y;
  static uint16_t  freqBin;
  static uint8_t  r, g, b;


  switch(LED_state) {
    case fft_bot_st:
      while (isr_flag) {
        if (fft.available()) {
          freqBin = 0;
          frameCount = isDynamic_onColor ? frameCount + 1 : frameCount;
          frameCountOff = isDynamic_offColor ? frameCountOff + 1 : frameCountOff;
          for (x = 0; x < WIDTH; x++) {

            level = fft.read(freqBin, freqBin + freqBins2[x] - 1);
            for (y = 1; y < HEIGHT; y++) {

              if (level >= thresholdVertical[y]) {
                leds.setPixel(map_xy_bot(x, y),
                  pick_color(x * 4, frameCount, fixedOnColor, isRainbow_onColor));
                //leds.setPixel(map_xy_bot(x, y), onColor[onColorInd]);
              } else {
                leds.setPixel(map_xy_bot(x, y),
                  pick_color(x * 4, frameCountOff, fixedOffColor, isRainbow_offColor));
              }
            }
            //fix always on LEDs
            leds.setPixel(map_xy_bot(x, 0),
              pick_color(x * 4, frameCount, fixedOnColor, isRainbow_onColor));
            freqBin = freqBin + freqBins2[x];
          }
        }
        leds.show();
      }
      break;

    case fft_top_st:
      while (isr_flag) {
        if (fft.available()) {
          frameCount = isDynamic_onColor ? frameCount + 1 : frameCount;
          frameCountOff = isDynamic_offColor ? frameCountOff + 1 : frameCountOff;
          freqBin = 0;
          for (x = 0; x < WIDTH; x++) {
            level = fft.read(freqBin, freqBin + freqBins2[x] - 1);

            for (y = 1; y < HEIGHT; y++) {
              if (level >= thresholdVertical[y]) {
                leds.setPixel(map_xy_top(x, y),
                  pick_color(x * 4, frameCount, fixedOnColor, isRainbow_onColor));
              } else {
                leds.setPixel(map_xy_top(x, y),
                  pick_color(x * 4, frameCountOff, fixedOffColor, isRainbow_offColor));
              }
            }
            //fix always on LEDs
            leds.setPixel(map_xy_top(x, 0),
              pick_color(x * 4, frameCount, fixedOnColor, isRainbow_onColor));
            freqBin = freqBin + freqBins2[x];
          }
        }
        leds.show();
      }
      break;

    case fft_btb_st:
      while (isr_flag) {
        if (fft.available()) {
          frameCount = isDynamic_onColor ? frameCount + 1 : frameCount;
          frameCountOff = isDynamic_offColor ? frameCountOff + 1 : frameCountOff;
          freqBin = 0;
          for (x = 0; x < WIDTH; x++) {
            level = fft.read(freqBin, freqBin + freqBins2[x] - 1);
            for (y = 1; y < HEIGHT / 2; y++) {
              if (level >= thresholdVertical[(y * 2) + 1]) {
                leds.setPixel(map_xy_bot(x, y),
                    pick_color(x * 4, frameCount, fixedOnColor, isRainbow_onColor));
                leds.setPixel(map_xy_top(x, y),
                    pick_color(x * 4, frameCount, fixedOnColor, isRainbow_onColor));
              } else {
                leds.setPixel(map_xy_bot(x, y),
                  pick_color(x * 4, frameCountOff, fixedOffColor, isRainbow_offColor));
                leds.setPixel(map_xy_top(x, y),
                  pick_color(x * 4, frameCountOff, fixedOffColor, isRainbow_offColor));
              }
            }
            // Fix always on LEDs
            leds.setPixel(map_xy_bot(x, 0),
              pick_color(x * 4, frameCount, fixedOnColor, isRainbow_onColor));
            leds.setPixel(map_xy_top(x, 0),
              pick_color(x * 4, frameCount, fixedOnColor, isRainbow_onColor));
            freqBin = freqBin + freqBins2[x];
          }
        }
        leds.show();
      }
      break;

    case fft_btbF_st:
      while (isr_flag) {
        if (fft.available()) {
          frameCount = isDynamic_onColor ? frameCount + 1 : frameCount;
          frameCountOff = isDynamic_offColor ? frameCountOff + 1 : frameCountOff;
          freqBin = 0;
          for (x = 0; x < WIDTH; x++) {
            level = fft.read(freqBin, freqBin + freqBins2[x] - 1);
            for (y = 1; y < HEIGHT / 2; y++) {
              if (level >= thresholdVertical[(y * 2) + 1]) {
                leds.setPixel(map_xy_bot(x, y),
                    pick_color(x * 4, frameCount, fixedOnColor, isRainbow_onColor));
                leds.setPixel(map_xy_top_F(x, y),
                    pick_color(x * 4, frameCount, fixedOnColor, isRainbow_onColor));
              } else {
                leds.setPixel(map_xy_bot(x, y),
                  pick_color(x * 4, frameCountOff, fixedOffColor, isRainbow_offColor));
                leds.setPixel(map_xy_top_F(x, y),
                  pick_color(x * 4, frameCountOff, fixedOffColor, isRainbow_offColor));
              }
            }
            // Fix always on LEDs
            leds.setPixel(map_xy_bot(x, 0),
              pick_color(x * 4, frameCount, fixedOnColor, isRainbow_onColor));
            leds.setPixel(map_xy_top_F(x, 0),
              pick_color(x * 4, frameCount, fixedOnColor, isRainbow_onColor));
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
          frameCount = isDynamic_onColor ? frameCount + 1 : frameCount;
          frameCountOff = isDynamic_offColor ? frameCountOff + 1 : frameCountOff;

          for (x = 0; x < WIDTH; x++) {
            level = fft.read(freqBin, freqBin + freqBins2[x] - 1);
            for (y = 0; y < HEIGHT / 2 - 1; y++) {
              if (level >= thresholdVertical[(y * 2) + 1] +
                             (freqBins2[x] * thresholdVertical[0])) {
                leds.setPixel(map_xy_midBot(x, y, HEIGHT / 2 + 1),
                              pick_color(x * 4, frameCount, fixedOnColor, isRainbow_onColor));
                leds.setPixel(map_xy_midTop(x, y, HEIGHT / 2 + 1),
                              pick_color(x * 4, frameCount, fixedOnColor, isRainbow_onColor));
              }
              else {
                leds.setPixel(map_xy_midBot(x, y, HEIGHT / 2 + 1),
                  pick_color(x * 4, frameCountOff, fixedOffColor, isRainbow_offColor));
                leds.setPixel(map_xy_midTop(x, y, HEIGHT / 2 + 1),
                  pick_color(x * 4, frameCountOff, fixedOffColor, isRainbow_offColor));
              }
            }
            // Fixed, center LEDs
            leds.setPixel(map_xy_midBot(x, 0, HEIGHT / 2),
                          pick_color(x * 4, frameCount, fixedOnColor, isRainbow_onColor));
            leds.setPixel(map_xy_midTop(x, 0, HEIGHT / 2),
                          pick_color(x * 4, frameCount, fixedOnColor, isRainbow_onColor));
            freqBin = freqBin + freqBins2[x];
          }
          leds.show();
        }
      }
      break;


    case fft_side_st:
    // need to blank out center strip
    if (isr_flag) {
      off();
    }
      while (isr_flag) {
        if (fft.available()) {
          frameCount = isDynamic_onColor ? frameCount + 1 : frameCount;
          frameCountOff = isDynamic_offColor ? frameCountOff + 1 : frameCountOff;
          freqBin = 0;
          for (y = 0; y < HEIGHT; y++) {
            level = fft.read(freqBin, freqBin + freqBins3[y] - 1);
              for (x = 1; x < WIDTH / 2; x++) {
              if (level >= thresholdHorizontal[(x * 2) + 1]) {
                leds.setPixel(map_xy_sidL(x, y),
                    pick_color(y * 6, frameCount, fixedOnColor, isRainbow_onColor));
                leds.setPixel(map_xy_sidR(x, y),
                    pick_color(y * 6, frameCount, fixedOnColor, isRainbow_onColor));
              } else {
                leds.setPixel(map_xy_sidL(x, y),
                  pick_color(y * 6, frameCountOff, fixedOffColor, isRainbow_offColor));
                leds.setPixel(map_xy_sidR(x, y),
                  pick_color(y * 6, frameCountOff, fixedOffColor, isRainbow_offColor));
              }
            }
            // Fixed, center LEDs
            leds.setPixel(map_xy_sidL(0, y),
                          pick_color(y * 6, frameCount, fixedOnColor, isRainbow_onColor));
            leds.setPixel(map_xy_sidR(0, y),
                          pick_color(y * 6, frameCount, fixedOnColor, isRainbow_onColor));
            freqBin = freqBin + freqBins3[y];
          }
        }
        leds.show();
      }
      break;

    case fft_sideF_st:
    // need to blank out center strip
    if (isr_flag) {
      off();
    }
      while (isr_flag) {
        if (fft.available()) {
          frameCount = isDynamic_onColor ? frameCount + 1 : frameCount;
          frameCountOff = isDynamic_offColor ? frameCountOff + 1 : frameCountOff;
          freqBin = 0;
          for (y = 0; y < HEIGHT; y++) {
            level = fft.read(freqBin, freqBin + freqBins3[y] - 1);
              for (x = 1; x < WIDTH / 2; x++) {
              if (level >= thresholdHorizontal[(x * 2) + 1]) {
                leds.setPixel(map_xy_sidL(x, y),
                  pick_color(y * 6, frameCount, fixedOnColor, isRainbow_onColor));
                leds.setPixel(map_xy_sidR_F(x, y),
                  pick_color(y * 6, frameCount, fixedOnColor, isRainbow_onColor));
              } else {
                leds.setPixel(map_xy_sidL(x, y),
                  pick_color(y * 6, frameCountOff, fixedOffColor, isRainbow_offColor));
                leds.setPixel(map_xy_sidR_F(x, y),
                  pick_color(y * 6, frameCountOff, fixedOffColor, isRainbow_offColor));
              }
            }
            // Fixed, center LEDs
            leds.setPixel(map_xy_sidL(0, y),
                          pick_color(y * 6, frameCount, fixedOnColor, isRainbow_onColor));
            leds.setPixel(map_xy_sidR_F(0, y),
                          pick_color(y * 6, frameCount, fixedOnColor, isRainbow_onColor));
            freqBin = freqBin + freqBins3[y];
          }
        }
        leds.show();
      }
      break;

    case rainbow_st:
      if (isr_flag) {
        off();
      }
      while (isr_flag) {
        frameCount++;
        for (x = 1; x < WIDTH - 1; x++) {
          for (y = 1; y < HEIGHT - 1; y++) {
            leds.setPixel(map_xy_bot(x, y),
                        rainbowColors[(x * 4 + y * 2 + frameCount) % COLOR_GRADIENT]);
          }
        }
        delay(40);
        leds.show();
      }
      break;

    case plaz_st:
      if (isr_flag) {
        off();
      }
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
}

void init_gridState(uint8_t LED_state)
{
  uint8_t   x, y;
  static uint8_t prev_state = off_st;
  static uint32_t prevOnColorInd = onColorInd + 1;
  static uint32_t prevOffColorInd = offColorInd + 1;

  if ((LED_state == prev_state) &&
      (prevOnColorInd == onColorInd) &&
      (prevOffColorInd == offColorInd)) {
    return;
  }

  prev_state = LED_state;
  prevOnColorInd = onColorInd;
  prevOffColorInd = offColorInd;

  off();
  switch (LED_state) {
    case fft_bot_st:
      for (x = 0; x < WIDTH; x++) {
        leds.setPixel(map_xy_bot(x, 0), onColor[onColorInd]);
      }
      for (x = 0; x < WIDTH; x++) {
        for (y = 1; y < HEIGHT; y++) {
          leds.setPixel(map_xy_bot(x, y), offColor[offColorInd]);
        }
      }
      leds.show();
      break;

      case fft_top_st:
        for (x = 0; x < WIDTH; x++) {
          leds.setPixel(map_xy_top(x, 0), rainbowColors[x*5]);
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




void off() {
  Serial.println("inside");
  uint8_t x, y;
  for (x = 0; x < WIDTH; x++) {
    for (y = 0; y < HEIGHT; y++) {
      leds.setPixel(map_xy_bot(x, y), BLACK);
    }
  }
  leds.show();
}


void testAll(uint8_t repeat) {
  uint8_t x, y;
  while (repeat && isr_flag) {
    for (x = 0; x < WIDTH; x++) {
      for (y = 0; y < HEIGHT ; y++) {
        leds.setPixel(map_xy_bot(x, y), GRAY);
        leds.show();
        delay(2);
      }
      delay(10);
      for (y = 0; y < HEIGHT ; y++) {
        leds.setPixel(map_xy_bot(x, y), BLACK);
      }
      leds.show();
    }
  }
}


/*
void init_gridState(uint8_t LED_state)
{
uint8_t   x, y;
static uint8_t prev_state = off_st;
static uint32_t prevOnColorInd = onColorInd + 1;
static uint32_t prevOffColorInd = offColorInd + 1;

if ((LED_state == prev_state) &&
  (prevOnColorInd == onColorInd) &&
  (prevOffColorInd == offColorInd)) {
return;
}

prev_state = LED_state;
prevOnColorInd = onColorInd;
prevOffColorInd = offColorInd;

off();
switch (LED_state) {
case fft_bot_st:
  for (x = 0; x < WIDTH; x++) {
    leds.setPixel(map_xy_bot(x, 0), onColor[onColorInd]);
  }
  for (x = 0; x < WIDTH; x++) {
    for (y = 1; y < HEIGHT; y++) {
      leds.setPixel(map_xy_bot(x, y), offColor[offColorInd]);
    }
  }
  leds.show();
  break;

  case fft_top_st:
    for (x = 0; x < WIDTH; x++) {
      leds.setPixel(map_xy_top(x, 0), rainbowColors[x*5]);
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
*/
