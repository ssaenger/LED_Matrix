//------------------------------------------------------------------------------
// The difference between one color to the next. The higher the value,
// the more the colors "blend" into eachother.
// Value should be between 360-0
#define RB_GRADIENT 36

// Saturation of the color. Sets the intensity of the color.
// Value should be between 100-0
// 100 = vivid colors, 0 = darker colors
#define SATURATION 100

// The brightness of the color. The higher the value, the whiter it looks
// Value should be between 100-0
// 100 = white, 50 = good balance, 0 = black
#define LIGHTNESS 50


// Holds the RGB values filled up by a call to makeColor()
// Don't change this value unless you modify makeColor()
uint32_t rainbowColors[RB_GRADIENT];

void setup() {
  makeColor(SATURATION, LIGHTNESS, rainbowColors);

}

void loop() {
  // put your main code here, to run repeatedly:

}
