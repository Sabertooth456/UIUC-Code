#include "color.hpp"

#include "fstream"

const int kMaxColorValue = 255;

Color::Color(int r, int g, int b) : red_(r), green_(g), blue_(b) {
  // need to ensure valid color has been constructed...
  if (red_ < 0 || red_ > kMaxColorValue) {
    throw std::runtime_error("Invalid Red");
  }
  if (green_ < 0 || green_ > kMaxColorValue) {
    throw std::runtime_error("Invalid Green");
  }
  if (blue_ < 0 || blue_ > kMaxColorValue) {
    throw std::runtime_error("Invalid Blue");
  }
}

// do not modify
bool operator==(const Color& rhs, const Color& lhs) {
  return (rhs.Red() == lhs.Red() && rhs.Green() == lhs.Green() &&
          rhs.Blue() == lhs.Blue());
}