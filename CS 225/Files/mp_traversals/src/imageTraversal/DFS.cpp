#include <iterator>
#include <cmath>

#include <list>
#include <queue>
#include <stack>
#include <vector>

#include "cs225/PNG.h"
#include "../Point.h"

#include "ImageTraversal.h"
#include "DFS.h"

/**
 * Default constructor
*/
DFS::DFS() {
  png_ = cs225::PNG();
  start_ = Point();
  starting_pixel_ = cs225::HSLAPixel();
  tolerance_ = 0;
  width_ = 0;
  height_ = 0;
}

/**
 * Initializes a depth-first ImageTraversal on a given `png` image,
 * starting at `start`, and with a given `tolerance`.
 * 
 * @param png The image this DFS is going to traverse
 * @param start The start point of this DFS
 * @param tolerance If the current point is too different (difference larger than tolerance) with the start point,
 * it will not be included in this DFS
 */
DFS::DFS(const PNG & png, const Point & start, double tolerance) {  
  /** @todo [Part 1] */
  png_ = png;
  start_ = start;
  starting_pixel_ = png_.getPixel(start_.x, start.y);
  tolerance_ = tolerance;
  add(start_);
  width_ = png.width();
  height_ = png.height();
}

/**
 * Returns an iterator for the traversal starting at the first point.
 */
ImageTraversal::Iterator DFS::begin() {
  /** @todo [Part 1] */
  DFS * dfs = new DFS(png_, start_, tolerance_);
  return ImageTraversal::Iterator(*dfs);
}

/**
 * Returns an iterator for the traversal one past the end of the traversal.
 */
ImageTraversal::Iterator DFS::end() {
  /** @todo [Part 1] */
  DFS * dfs = new DFS();
  return ImageTraversal::Iterator(*dfs);
}

/**
 * Adds a Point for the traversal to visit at some point in the future.
 */
void DFS::add(const Point & point) {
  /** @todo [Part 1] */
  nodes_to_visit.push(point);
  // nodes_visited[std::tuple<unsigned int, unsigned int>(point.x, point.y)] = true;
}

/**
 * Adds the node's neighbors to the depth first search, if they are valid
 * @param point The point whose neighbors will be added for the traversal to visit at some point in the future
*/
void DFS::addNeighbors(const Point & point) {
  // Since the height and width are unsigned, if x == 0, x - 1 == int.MAX_VALUE, which is greater than width_
  // Same applies for height_
  //  T
  // L*R
  //  B
  if (!isValidPoint(point)) {
    return;
  }
  if (point.x < width_ - 1) { // Checks if the right side of the pixel is valid
    Point right_pixel = Point(point.x + 1, point.y);
    if (isNewNode(right_pixel) && isValidPoint(right_pixel)) {
      add(right_pixel);
    }
  }
  if (point.y < height_ - 1) { // Checks if the bottom side of the pixel is valid
    Point bottom_pixel = Point(point.x, point.y + 1);
    if (isNewNode(bottom_pixel) && isValidPoint(bottom_pixel)) {
      add(bottom_pixel);
    }
  }
  if (point.x > 0) { // Checks if the left side of the pixel is valid
    Point left_pixel = Point(point.x - 1, point.y);
    if (isNewNode(left_pixel) && isValidPoint(left_pixel)) {
      add(left_pixel);
    }
  }
  if (point.y > 0) { // Checks if the top side of the pixel is valid
    Point top_pixel = Point(point.x, point.y - 1);
    if (isNewNode(top_pixel) && isValidPoint(top_pixel)) {
      add(top_pixel);
    }
  }
}

// Assumes point.x and point.y are less than width_ and height_
bool DFS::isNewNode(const Point & point) {
  return isNewNode(point.x, point.y);
}

bool DFS::isNewNode(unsigned int x, unsigned int y) {
  return !nodes_visited[std::tuple<unsigned int, unsigned int>(x, y)]; // If the node has been visited (true), it is not a new node
}

bool DFS::isValidPoint(const Point & point) {
  if (point.x >= width_ || point.y >= height_) {
    return false;
  }
  return tolerance_ >= calculateDelta(starting_pixel_, png_.getPixel(point.x, point.y));
}

/**
 * Removes and returns the current Point in the traversal.
 */
Point DFS::pop() {
  /** @todo [Part 1] */
  if (empty()) {
    std::cout << "DFS pop(): Out of nodes to return" << std::endl;
    return Point();
  }
  Point point = nodes_to_visit.top();
  // if (nodes_visited[std::tuple<unsigned int, unsigned int>(point.x, point.y)] == true) {
  //   nodes_to_visit.pop();
  //   return pop();
  // }
  nodes_visited[std::tuple<unsigned int, unsigned int>(point.x, point.y)] = true;
  nodes_to_visit.pop();
  clearDuplicates();
  return point;
}

void DFS::clearDuplicates() {
  if (empty()) {
    return;
  }
  Point point = nodes_to_visit.top();
  while (nodes_visited[std::tuple<unsigned int, unsigned int>(point.x, point.y)] == true) {
    nodes_to_visit.pop();
    if (empty()) {
      return;
    }
    point = nodes_to_visit.top();
  }
}

/**
 * Returns the current Point in the traversal.
 */
Point DFS::peek() const {
  /** @todo [Part 1] */
  if (empty()) {
    // std::cout << "DFS peek(): Out of nodes to return" << std::endl;
    return Point();
  }
  return nodes_to_visit.top();
}

/**
 * Returns true if the traversal is empty.
 */
bool DFS::empty() const {
  /** @todo [Part 1] */
  return nodes_to_visit.empty();
}
