
// While I may want to play around with the width to turn on LEDs in,
// this constant will never change, since my LED matrix's width will
// never change.
#define LED_FIXED_WIDTH  47
#define LED_FIXED_HEIGHT 60

// Covers whole spectrum
static uint32_t FreqBins1[LED_FIXED_WIDTH] = {
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    2,  2,  2,  2,  2,  2,  2,  2,  3,  3,
    3,  3,  4,  4,  4,  4,  5,  5,  5,  5,
    6,  6,  7,  7,  8,  9, 11, 14, 18, 22,
   25, 30, 32, 36, 40, 47, 53
};

// Concerned with lower frequencies
static uint32_t FreqBins2[LED_FIXED_WIDTH] = {
   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
   2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
   3,  3,  3,  3,  3,  4,  4,  4,  4,  4,
   5,  5,  5,  5,  5,  6,  6,  6,  6,  6,
   7,  7,  7,  7,  8,  8,  8
};

// Use for ff_sid_st
static uint32_t FreqBins3[LED_FIXED_HEIGHT] = {
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    2,  2,  2,  2,  2,  2,  2,  2,  2,  3,
    3,  3,  3,  3,  4,  4,  4,  4,  4,  5,
    5,  5,  6,  6,  6,  7,  7,  7,  8,  8,
    9,  9, 10, 10, 11, 12, 12, 13, 14, 15,
   15, 16, 17, 18, 19, 20, 22, 23, 24, 25
};


uint32_t* spectrum_getBin1() {
  return FreqBins1;
}

uint32_t* spectrum_getBin2() {
  return FreqBins2;
}

uint32_t* spectrum_getBin3() {
  return FreqBins3;
}
