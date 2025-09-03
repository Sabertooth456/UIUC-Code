/**
 * @file kdtree.cpp
 * Implementation of KDTree class.
 */

#include <utility>
#include <algorithm>
#include <cmath>

using namespace std;

template <int Dim>
bool KDTree<Dim>::smallerDimVal(const Point<Dim>& first,
                                const Point<Dim>& second, int curDim) const
{
    /**
     * @todo Implement this function!
     */
    if (first[curDim] < second[curDim]) {
      return true;
    } else if (first[curDim] > second[curDim]) {
      return false;
    }
    return first < second;
}

template <int Dim>
bool KDTree<Dim>::shouldReplace(const Point<Dim>& target,
                                const Point<Dim>& currentBest,
                                const Point<Dim>& potential) const
{
    /**
     * @todo Implement this function!
     */
    double currentDistanceSquared = 0;
    double potentialDistanceSquared = 0;
    for (int d = 0; d < Dim; d++) {
      currentDistanceSquared += (currentBest[d] - target[d]) * (currentBest[d] - target[d]);
      potentialDistanceSquared += (potential[d] - target[d]) * (potential[d] - target[d]);
    }
    if (potentialDistanceSquared < currentDistanceSquared) {
      return true;
    } else if (potentialDistanceSquared > currentDistanceSquared) {
      return false;
    }
    return potential < currentBest;
}

template <int Dim>
KDTree<Dim>::KDTree(const vector<Point<Dim>>& newPoints)
{
    /**
     * @todo Implement this function!
     */
    
    if (newPoints.empty()) {
      size = 0;
      root = nullptr;
      return;
    }
    size = newPoints.size();
    vector<Point<Dim>> pointsCopy = newPoints;
    root = KDTreeConstructor(pointsCopy, 0, size - 1, 0);
}

template <int Dim>
typename KDTree<Dim>::KDTreeNode* KDTree<Dim>::KDTreeConstructor(vector<Point<Dim>>& points, int lowerBound, int upperBound, int d)
{
    /**
     * @todo Implement this function!
     */
    if (lowerBound > upperBound) {
      return nullptr;
    } else if (lowerBound == upperBound) {
      return (new KDTreeNode(points[lowerBound]));
    } else {
      int midpoint = (upperBound - lowerBound) / 2 + lowerBound;
      sort(points, lowerBound, upperBound, midpoint, d);
      KDTreeNode* parent = new KDTreeNode(points[midpoint]);
      parent->left = KDTreeConstructor(points, lowerBound, midpoint - 1, (d + 1) % Dim);
      parent->right = KDTreeConstructor(points, midpoint + 1, upperBound, (d + 1) % Dim);
      return parent;
    }
}

/**
 * Returns the k-th smallest element within the lowerBound and upperBound inclusive with respect to the Point's d dimension
 * and sorts the array so that the smaller values are to the left of k
 * @param points The list of points to be sorted
 * @param lowerBound The lower bound to be searched
 * @param upperBound The upper bound to be searched
 * @param k The k-th smallest element
 * @param d Which dimension that will be used to find the k-th smallest element
*/
template <int Dim>
void KDTree<Dim>::sort(vector<Point<Dim>>& points, int lowerBound, int upperBound, int k, int d)
{
    /**
     * @todo Implement this function!
     * Implement using quickselect
     */
    if (lowerBound == upperBound) { // If the list contains only 1 element
        return;                     // the list is already partitioned
    }
    int pivotIndex = partition(points, lowerBound, upperBound, lowerBound, d);
    if (pivotIndex == k) {
      return;
    } else if (k < pivotIndex) {
      sort(points, lowerBound, pivotIndex - 1, k, d);
    } else {
      sort(points, pivotIndex + 1, upperBound, k, d);
    }
}

/**
 * This function splits the list into values less than and greater than the pivot index.
 * @return The pivot index's sorted position in the list
*/
template <int Dim>
int KDTree<Dim>::partition(vector<Point<Dim>>& points, int lowerBound, int upperBound, int pivotIndex, int d) {
    Point<Dim> pivot = points[pivotIndex];
    swap(points[pivotIndex], points[upperBound]); // This moves the pivotIndex to the back of the array
    int curIndex = lowerBound;
    for (int i = lowerBound; i <= upperBound - 1; ++i) {
        if (smallerDimVal(points[i], pivot, d)) {
            swap(points[curIndex], points[i]);
            ++curIndex;
        }
    }
    swap(points[curIndex], points[upperBound]);
    return curIndex;
}

template <int Dim>
KDTree<Dim>::KDTree(const KDTree<Dim>& other) {
  /**
   * @todo Implement this function!
   */
  size = other.size;
  copy_(other.root, root);
}

template <int Dim>
void KDTree<Dim>::copy_(KDTreeNode* curNode, KDTreeNode*& parent) {
  if (curNode == nullptr) {
    parent = nullptr;
  } else {
    parent = new KDTreeNode(curNode->point);
    copy_(curNode->left, parent->left);
    copy_(curNode->right, parent->right);
  }
}

template <int Dim>
const KDTree<Dim>& KDTree<Dim>::operator=(const KDTree<Dim>& rhs) {
  /**
   * @todo Implement this function!
   */
  ~KDTree();
  size = rhs.size;
  copy_(rhs.root, root);
  return *this;
}

template <int Dim>
KDTree<Dim>::~KDTree() {
  /**
   * @todo Implement this function!
   */
  clear_(root);
  size = 0;
  root = nullptr;
}

template <int Dim>
void KDTree<Dim>::clear_(KDTreeNode* curNode) {
  if (curNode == nullptr) { return; }
  if (curNode->left != nullptr) {
    clear_(curNode->left);
    curNode->left = nullptr;
  }
  if (curNode->right != nullptr) {
    clear_(curNode->right);
    curNode->right = nullptr;
  }
  delete curNode;
  curNode = nullptr;
}

template <int Dim>
Point<Dim> KDTree<Dim>::findNearestNeighbor(const Point<Dim>& query) const
{
    /**
     * @todo Implement this function!
     */
    return findNearestNeighbor(query, root, 0);
}

template <int Dim>
Point<Dim> KDTree<Dim>::findNearestNeighbor(const Point<Dim>& query, KDTreeNode* curNode, int curDim) const {
  if (curNode == nullptr) {
    return Point<Dim>();
  }
  if (curNode->left == nullptr && curNode->right == nullptr) {
    return curNode->point;
  }
  Point<Dim> neighbor = curNode->point;
  Point<Dim> newNeighbor = curNode->point;
  if (smallerDimVal(query, curNode->point, curDim) && curNode->left != nullptr) {
    newNeighbor = findNearestNeighbor(query, curNode->left, (curDim + 1) % Dim);
  } else if (curNode->right != nullptr) {
    newNeighbor = findNearestNeighbor(query, curNode->right, (curDim + 1) % Dim);
  }
  if (shouldReplace(query, neighbor, newNeighbor)) {
    neighbor = newNeighbor;
  }
  double distanceSquared = distance(query, neighbor); // radius^2 >= curDim_distance^2
  if (distanceSquared >= (curNode->point[curDim] - query[curDim]) * (curNode->point[curDim] - query[curDim])) {
    if (smallerDimVal(query, curNode->point, curDim) && curNode->right != nullptr) {
      newNeighbor = findNearestNeighbor(query, curNode->right, (curDim + 1) % Dim);
    } else if (curNode->left != nullptr) {
      newNeighbor = findNearestNeighbor(query, curNode->left, (curDim + 1) % Dim);
    }
    if (shouldReplace(query, neighbor, newNeighbor)) {
      neighbor = newNeighbor;
    }
  }
  return neighbor;
}

template <int Dim>
double KDTree<Dim>::distance(const Point<Dim>& pointA, const Point<Dim>& pointB) const {
  double distanceSquared = 0.0;
  for (int d = 0; d < Dim; ++d) {
    distanceSquared += (pointA[d] - pointB[d]) * (pointA[d] - pointB[d]);
  }
  return distanceSquared;
}