
// While I may want to play around with the width to turn on LEDs in,
// this constant will never change, since my LED matrix's width will
// never change.
#define LED_FIXED_WIDTH 47

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
   3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
   4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
   5,  5,  5,  5,  5,  5,  5
};


uint32_t* spectrum_getBin1() {
  return FreqBins1;
}


uint32_t* spectrum_getBin2() {
  return FreqBins2;
}
