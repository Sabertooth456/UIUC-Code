#include <cmath>
#include <iterator>
#include <iostream>

#include "cs225/HSLAPixel.h"
#include "cs225/PNG.h"
#include "../Point.h"

#include "ImageTraversal.h"

/**
 * Calculates a metric for the difference between two pixels, used to
 * calculate if a pixel is within a tolerance.
 *
 * @param p1 First pixel
 * @param p2 Second pixel
 * @return the difference between two HSLAPixels
 */
double ImageTraversal::calculateDelta(const HSLAPixel & p1, const HSLAPixel & p2) {
  double h = fabs(p1.h - p2.h);
  double s = p1.s - p2.s;
  double l = p1.l - p2.l;

  // Handle the case where we found the bigger angle between two hues:
  if (h > 180) { h = 360 - h; }
  h /= 360;

  return sqrt( (h*h) + (s*s) + (l*l) );
}

/**
 * Default iterator constructor.
 */
ImageTraversal::Iterator::Iterator() {
  /** @todo [Part 1] */
}

ImageTraversal::Iterator::~Iterator() {
  // Do nothing?
}

ImageTraversal::Iterator::Iterator(ImageTraversal & image) {
  image_ = &image;
}

// /**
//  * Iterator constructor.
//  * @param cur The current position of the iterator
//  */
// ImageTraversal::Iterator::Iterator(Point cur) {
//   position_ = cur;
// }

/**
 * Iterator pre-increment operator.
 *
 * Advances the traversal of the image.
 */
ImageTraversal::Iterator & ImageTraversal::Iterator::operator++() {
  /** @todo [Part 1] */
  Point cur_point = image_->pop();
  image_->addNeighbors(cur_point);
  return *this;
}

// /**
//  * Iterator post-increment operator.
//  *
//  * Advances the traversal of the image.
//  */
// ImageTraversal::Iterator & ImageTraversal::Iterator::operator++(int) {
//   /** @todo [Part 1] */
//   return *this;
// }

/**
 * Iterator accessor opreator.
 *
 * Accesses the current Point in the ImageTraversal.
 */
Point ImageTraversal::Iterator::operator*() {
  /** @todo [Part 1] */
  return image_->peek();
}

/**
 * Iterator inequality operator.
 *
 * Determines if two iterators are not equal.
 */
bool ImageTraversal::Iterator::operator!=(const ImageTraversal::Iterator &other) {
  /** @todo [Part 1] */
  if (image_ == nullptr && other.image_ == nullptr) {
    return false;
  } else if (image_ == nullptr || other.image_ == nullptr) {
    return true;
  }
  const Point position = image_->peek();
  const Point other_position = other.image_->peek();
  return !((position.x == other_position.x) && (position.y == other_position.y) && (image_->empty() && image_->empty()));
}

