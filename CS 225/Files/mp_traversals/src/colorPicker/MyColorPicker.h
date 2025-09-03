#pragma once

#include "ColorPicker.h"
#include "cs225/HSLAPixel.h"
#include "../Point.h"

using namespace cs225;

/**
 * A color picker class using your own color picking algorithm
 */
class MyColorPicker : public ColorPicker {
public:
  MyColorPicker(unsigned increment1, unsigned increment2, unsigned scale);
  HSLAPixel getColor(unsigned x, unsigned y);

private:
  unsigned increment1_;
  unsigned increment2_;
  unsigned scale_;
};
