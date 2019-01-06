
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
    5,  5,  6,  6,  6,  6,  6,  6,  6,  6,
    6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
    6,  6,  6,  6,  6,  6,  6,  6,  6,  6
};

// Looking at the source code, the sum of the freqBins array shouldn't be over 511
// for some reason. So I'll check for that here, and remove it in the source code.
uint32_t verifyMaxBin(uint32_t* arr) {
  uint32_t n = sizeof(arr) / sizeof(arr[0]);
  uint32_t sum = 0;
  uint32_t i;

  for (i = 0; i < n; i++) {
    sum += arr[i];
  }
  return sum;
}

uint32_t* spectrum_getBin1() {
  uint32_t sum = verifyMaxBin(FreqBins1);
  if (sum >= BIN_LENGTH) {
    Serial.println("ERROR. FreqBins1 summed values is too high!");
    return 0;
  }
  return FreqBins1;
}

uint32_t* spectrum_getBin2() {
  uint32_t sum = verifyMaxBin(FreqBins2);
  if (sum >= BIN_LENGTH) {
    Serial.println("ERROR. FreqBins2 summed values is too high!");
    return 0;
  }
  return FreqBins2;
}

uint32_t* spectrum_getBin3() {
  uint32_t sum = verifyMaxBin(FreqBins3);
  if (sum >= BIN_LENGTH) {
    Serial.println("ERROR. FreqBins3 summed values is too high!");
    return 0;
  }
  return FreqBins3;
}
