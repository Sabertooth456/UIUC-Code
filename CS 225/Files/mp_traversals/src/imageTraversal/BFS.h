/**
 * @file BFS.h
 */

#pragma once

#include <iterator>
#include <cmath>
#include <list>
#include <queue>
#include <map>
#include <tuple>

#include "cs225/PNG.h"
#include "../Point.h"

#include "ImageTraversal.h"

using namespace cs225;

/**
 * A breadth-first ImageTraversal.
 * Derived from base class ImageTraversal
 */
class BFS : public ImageTraversal {
public:
  BFS();
  BFS(const PNG & png, const Point & start, double tolerance);

  ImageTraversal::Iterator begin();
  ImageTraversal::Iterator end();

  void add(const Point & point);
  void addNeighbors(const Point & point);
  Point pop();
  Point peek() const;
  bool empty() const;

private:
  /** @todo [Part 1] */
  /** add private members here*/

  /**
   * Checks if the point has already been visited
   * @param Point The point to be checked
  */
  bool isNewNode(const Point & point);
  bool isNewNode(unsigned int x, unsigned int y);
  bool isValidPoint(const Point & point);

  Point start_;
  cs225::HSLAPixel starting_pixel_;
  PNG png_;
  double tolerance_;
  std::queue<Point> nodes_to_visit;
  std::map<std::tuple<unsigned int, unsigned int>, bool> nodes_visited;
  unsigned int width_;
  unsigned int height_;
};
