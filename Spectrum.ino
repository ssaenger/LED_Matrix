
// While I may want to play around with the width to turn on LEDs in,
// this constant will never change, since my LED matrix's width will
// never change.
#define LED_FIXED_WIDTH  47
#define LED_FIXED_HEIGHT 60

// Covers whole spectrum

static uint32_t FreqBins0[LED_FIXED_WIDTH] = {
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
    3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
    3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
    3,  3,  3,  3,  3,  3,  3
};

static uint32_t FreqBins1[LED_FIXED_WIDTH] = {
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    2,  2,  2,  2,  2,  2,  2,  2,  3,  3,
    3,  3,  3,  3,  3,  4,  4,  4,  4,  4,
    4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
    5,  5,  5,  5,  5,  5,  5
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

#ifdef SHAWN_PATCH
// Looking at the source code, the sum of the freqBins array shouldn't be over 511
// for some reason. So I'll check for that here, and remove it in the source code.
uint32_t verifyMaxBin(uint32_t* arr, uint16_t binSize) {
  uint32_t sum = 0;
  uint32_t i;

  for (i = 0; i < binSize; i++) {
    sum += arr[i];
  }
  Serial.println(i);
  Serial.print("Bin sum: ");
  Serial.println(sum);
  return sum;
}
#endif // SHAWN_PATCH

uint32_t* spectrum_getBin0() {
#ifdef SHAWN_PATCH
  uint32_t sum = verifyMaxBin(FreqBins0, LED_FIXED_WIDTH);
  if (sum >= BIN_LENGTH) {
    Serial.println("ERROR. FreqBins1 summed values is too high!");
    return 0;
  }
#endif // SHAWN_PATCH

  return FreqBins0;
}

uint32_t* spectrum_getBin1() {
#ifdef SHAWN_PATCH
  uint32_t sum = verifyMaxBin(FreqBins1, LED_FIXED_WIDTH);
  if (sum >= BIN_LENGTH) {
    Serial.println("ERROR. FreqBins1 summed values is too high!");
    return 0;
  }
#endif // SHAWN_PATCH

  return FreqBins1;
}

uint32_t* spectrum_getBin2() {
#ifdef SHAWN_PATCH
  uint32_t sum = verifyMaxBin(FreqBins2, LED_FIXED_WIDTH);
  if (sum >= BIN_LENGTH) {
    Serial.println("ERROR. FreqBins2 summed values is too high!");
    return 0;
  }
#endif // SHAWN_PATCH

  return FreqBins2;
}

uint32_t* spectrum_getBin3() {
#ifdef SHAWN_PATCH
  uint32_t sum = verifyMaxBin(FreqBins3, LED_FIXED_HEIGHT);
  if (sum >= BIN_LENGTH) {
    Serial.println("ERROR. FreqBins3 summed values is too high!");
    return 0;
  }
#endif // SHAWN_PATCH
  return FreqBins3;
}
