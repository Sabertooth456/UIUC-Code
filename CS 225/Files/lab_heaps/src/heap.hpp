
/**
 * @file heap.cpp
 * Implementation of a heap class.
 */
#include <cstddef>

template <class T, class Compare>
class heap;

template <class T, class Compare>
size_t heap<T, Compare>::root() const
{
    // @TODO Update to return the index you are choosing to be your root.
    // This class is a 1-indexed heap
    return 1;
}

template <class T, class Compare>
size_t heap<T, Compare>::leftChild(size_t currentIdx) const
{
    // @TODO Update to return the index of the left child.
    size_t leftIdx = currentIdx * 2;
    if (leftIdx > _elems.size() - 1) {
        return 0;
    }
    return leftIdx;
}

template <class T, class Compare>
size_t heap<T, Compare>::rightChild(size_t currentIdx) const
{
    // @TODO Update to return the index of the right child.
    size_t rightIdx = currentIdx * 2 + 1;
    if (rightIdx > _elems.size() - 1) {
        return 0;
    }
    return rightIdx;
}

template <class T, class Compare>
size_t heap<T, Compare>::parent(size_t currentIdx) const
{
    // @TODO Update to return the index of the parent.
    if (currentIdx == root()) {
        return 0;
    }
    return currentIdx / 2;
}

template <class T, class Compare>
bool heap<T, Compare>::hasAChild(size_t currentIdx) const
{
    // @TODO Update to return whether the given node has a child
    return _elems.size() > currentIdx * 2;
}

template <class T, class Compare>
size_t heap<T, Compare>::maxPriorityChild(size_t currentIdx) const
{
    // @TODO Update to return the index of the child with highest priority
    ///   as defined by higherPriority()
    if (!hasAChild(currentIdx)) {
        return 0;
    }
    size_t leftIdx = leftChild(currentIdx);
    size_t rightIdx = rightChild(currentIdx);
    if (rightIdx == 0 || (leftIdx != 0 && higherPriority(_elems[leftIdx], _elems[rightIdx]))) {
        return leftIdx;
    }
    return rightIdx;
}

template <class T, class Compare>
void heap<T, Compare>::heapifyDown(size_t currentIdx)
{
    // @TODO Implement the heapifyDown algorithm.
    if (hasAChild(currentIdx)) {
        size_t leftIdx = leftChild(currentIdx);
        size_t rightIdx = rightChild(currentIdx);
        if (rightIdx == 0 || (leftIdx != 0 && higherPriority(_elems[leftIdx], _elems[rightIdx]))) {
            if (higherPriority(_elems[leftChild(currentIdx)], _elems[currentIdx])) {
                std::swap(_elems[currentIdx], _elems[leftChild(currentIdx)]);
                heapifyDown(leftChild(currentIdx));
            }
        } else {
            if (higherPriority(_elems[rightChild(currentIdx)], _elems[currentIdx])) {
                std::swap(_elems[currentIdx], _elems[rightChild(currentIdx)]);
                heapifyDown(rightChild(currentIdx));
            }
        }
    }
}

template <class T, class Compare>
void heap<T, Compare>::heapifyUp(size_t currentIdx)
{
    if (currentIdx == root()) {
        return;
    }
    size_t parentIdx = parent(currentIdx);
    if (higherPriority(_elems[currentIdx], _elems[parentIdx])) {
        std::swap(_elems[currentIdx], _elems[parentIdx]);
        heapifyUp(parentIdx);
    }
}

// template <class T, class Compare>
// void heap<T, Compare>::swap(size_t firstIdx, size_t secondIdx) {
//     T firstValue = _elems[firstIdx];
//     T secondValue = _elems[secondIdx];
//     _elems[firstIdx] = secondValue;
//     _elems[secondValue] = firstValue;
// }

template <class T, class Compare>
heap<T, Compare>::heap()
{
    // @TODO Depending on your implementation, this function may or may
    ///   not need modifying
    _elems.push_back(T());
}

template <class T, class Compare>
heap<T, Compare>::heap(const std::vector<T>& elems) 
{
    // @TODO Construct a heap using the buildHeap algorithm
    _elems.push_back(T());
    for (T e : elems) {
        _elems.push_back(e);
    }
    size_t start = parent(_elems.size());
    for (size_t idx = start; idx >= root(); idx--) {
        heapifyDown(idx);
    }
}

template <class T, class Compare>
T heap<T, Compare>::pop()
{
    // @TODO Remove, and return, the element with highest priority
    T elem = _elems[root()];
    std::swap(_elems[root()], _elems[_elems.size() - 1]);
    _elems.pop_back();
    heapifyDown(root());
    return elem;
}

template <class T, class Compare>
T heap<T, Compare>::peek() const
{
    // @TODO Return, but do not remove, the element with highest priority
    return _elems[root()];
}

template <class T, class Compare>
void heap<T, Compare>::push(const T& elem)
{
    // @TODO Add elem to the heap
    _elems.push_back(elem);
    heapifyUp(_elems.size() - 1);
}

template <class T, class Compare>
void heap<T, Compare>::updateElem(const size_t & idx, const T& elem)
{
    // @TODO In-place updates the value stored in the heap array at idx
    // Corrects the heap to remain as a valid heap even after update
    _elems[idx] = elem;
    heapifyDown(idx);
    heapifyUp(idx);
}


template <class T, class Compare>
bool heap<T, Compare>::empty() const
{
    // @TODO Determine if the heap is empty
    return _elems.size() <= 1;
}

template <class T, class Compare>
void heap<T, Compare>::getElems(std::vector<T> & heaped) const
{
    for (size_t i = root(); i < _elems.size(); i++) {
        heaped.push_back(_elems[i]);
    }
}
