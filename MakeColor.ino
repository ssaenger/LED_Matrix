// Convert HSL (Hue, Saturation, Lightness) to RGB (Red, Green, Blue)
//
//   hue:        0 to 359 - position on the color wheel, 0=red, 60=orange,
//                            120=yellow, 180=green, 240=blue, 300=violet
//
//   saturation: 0 to 100 - how bright or dull the color, 100=full, 0=gray
//
//   lightness:  0 to 100 - how light the color is, 100=white, 50=color, 0=black
//
int makeColor(uint32_t saturation, uint32_t lightness, uint32_t* rainbowColors)
{
  uint32_t = red, green, blue; 
  uint32_t var1, var2;
  uint16_t i;

  hue = (hue > 359) ? (hue % 360) : hue;
  //if (hue > 359) hue = hue % 360;
  saturation = (saturation > 100) ? 100 : saturation;
  //if (saturation > 100) saturation = 100;
  lightness = (lightness > 100) ? 100 : lightness;
  //if (lightness > 100) lightness = 100;


  // algorithm from: http://www.easyrgb.com/index.php?X=MATH&H=19#text19
  for (i = 0; i < RB_GRADIENT; i++) {
    if (saturation == 0) {
      red = lightness * 255 / 100;
      green = red;
      blue = red;
    } 
    else {
      if (lightness < 50) {
        var2 = lightness * (100 + saturation);
      } else {
        var2 = ((lightness + saturation) * 100) - (saturation * lightness);
      }
      var1 = lightness * 200 - var2;
      red = h2rgb(var1, var2, (hue < 240) ? hue + 120 : hue - 240) * 255 / 600000;
      green = h2rgb(var1, var2, hue) * 255 / 600000;
      blue = h2rgb(var1, var2, (hue >= 120) ? hue - 120 : hue + 240) * 255 / 600000;
    }
    rainbowColors[i] = (red << 16) | (green << 8) | blue;
  }
}

unsigned int h2rgb(unsigned int v1, unsigned int v2, unsigned int hue)
{
  if (hue < 60) return v1 * 60 + (v2 - v1) * hue;
  if (hue < 180) return v2 * 60;
  if (hue < 240) return v1 * 60 + (v2 - v1) * (240 - hue);
  return v1 * 60;
}

// alternate code:
// http://forum.pjrc.com/threads/16469-looking-for-ideas-on-generating-RGB-colors-from-accelerometer-gyroscope?p=37170&viewfull=1#post37170
