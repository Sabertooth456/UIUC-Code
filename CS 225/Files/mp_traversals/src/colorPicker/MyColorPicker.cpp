#include "cs225/HSLAPixel.h"
#include "../Point.h"

#include "ColorPicker.h"
#include "MyColorPicker.h"

using namespace cs225;

MyColorPicker::MyColorPicker(unsigned increment1, unsigned increment2, unsigned scale) {
  increment1_ = increment1;
  increment2_ = increment2;
  scale_ = scale;
}

/**
 * Picks the color for pixel (x, y).
 * Using your own algorithm
 */
HSLAPixel MyColorPicker::getColor(unsigned x, unsigned y) {
  /* @todo [Part 3] */
  unsigned smorgasbord = increment1_ * x / scale_ + increment2_ * y / scale_ + (increment1_ * x * increment2_ * y / scale_ / scale_) ;
  smorgasbord = smorgasbord % 360;
  HSLAPixel pixel(smorgasbord, 1, 0.5);
  return pixel;
}
