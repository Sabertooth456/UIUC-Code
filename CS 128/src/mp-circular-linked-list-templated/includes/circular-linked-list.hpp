#ifndef CIRCULAR_LINKED_LIST_HPP
#define CIRCULAR_LINKED_LIST_HPP

#include <iostream>

#include "node.hpp"

enum class Order { kASC, kDESC };

template <typename T>
class CircularLinkedList {
 public:
  CircularLinkedList() = default;
  CircularLinkedList(const CircularLinkedList<T>& source);
  CircularLinkedList<T>& operator=(const CircularLinkedList<T>& source);
  ~CircularLinkedList();

  void InsertInOrder(const T& data);
  void Reverse();

  bool BelongsBeforeThisValue(T data, T value_to_compare_with) const;

  template <typename U>
  friend std::ostream& operator<<(std::ostream& os,
                                  const CircularLinkedList<U>& cll);

 private:
  Node<T>* head_ = nullptr;
  Node<T>* tail_ = nullptr;
  Order node_order_ = Order::kASC;
};

template <typename T>
CircularLinkedList<T>::CircularLinkedList(const CircularLinkedList<T>& source) {
  node_order_ = source.node_order_;
  if (source.head_ == nullptr || source.tail_ == nullptr) {
    head_ = nullptr;
    tail_ = nullptr;
    return;
  }
  Node<T>* head = source.head_;
  Node<T>* node_to_copy = head;
  Node<T>* copied_node = new Node<T>(node_to_copy->data);
  head_ = copied_node;
  Node<T>* copy_to = head_;

  node_to_copy = node_to_copy->next;
  while (node_to_copy != head && node_to_copy != nullptr) {
    copied_node = new Node<T>(node_to_copy->data);
    copy_to->next = copied_node;
    copy_to = copy_to->next;
    node_to_copy = node_to_copy->next;
  }
  tail_ = copied_node;
  tail_->next = head_;
}

template <typename T>
CircularLinkedList<T>& CircularLinkedList<T>::operator=(
    const CircularLinkedList<T>& source) {
  if (this == &source) {
    return *this;
  }
  this->~CircularLinkedList();
  node_order_ = source.node_order_;
  if (source.head_ == nullptr || source.tail_ == nullptr) {
    head_ = nullptr;
    tail_ = nullptr;
    return *this;
  }
  Node<T>* head = source.head_;
  Node<T>* node_to_copy = head;
  Node<T>* copied_node = new Node<T>(node_to_copy->data);
  head_ = copied_node;
  Node<T>* copy_to = head_;

  node_to_copy = node_to_copy->next;
  while (node_to_copy != head && node_to_copy != nullptr) {
    copied_node = new Node<T>(node_to_copy->data);
    copy_to->next = copied_node;
    copy_to = copy_to->next;
    node_to_copy = node_to_copy->next;
  }
  tail_ = copied_node;
  tail_->next = head_;
  return *this;
}

template <typename T>
CircularLinkedList<T>::~CircularLinkedList() {
  if (head_ == nullptr) {
    return;
  }
  if (head_ == tail_) {
    delete head_;
    head_ = nullptr;
    tail_ = nullptr;
    return;
  }
  Node<T>* cur_node = head_;
  Node<T>* next_node = nullptr;
  tail_->next = nullptr;
  do {
    next_node = cur_node->next;
    delete cur_node;
    cur_node = next_node;
  } while (next_node != nullptr);
  head_ = nullptr;
  tail_ = nullptr;
}

template <typename T>
void CircularLinkedList<T>::InsertInOrder(const T& data) {
  // Empty list:
  if (head_ == nullptr) {
    head_ = new Node<T>(data);
    head_->next = head_;
    tail_ = head_;
    return;
  }
  // List with single item:
  if (head_ == tail_) {
    if (BelongsBeforeThisValue(data, head_->next->data)) {
      head_ = new Node<T>(data);
    } else {
      tail_ = new Node<T>(data);
    }
    head_->next = tail_;
    tail_->next = head_;
    return;
  }
  // If it belongs before the head_:
  if (BelongsBeforeThisValue(data, head_->data)) {
    tail_->next = new Node<T>(data, head_);
    head_ = tail_->next;

    return;
  }
  // Otherwise:
  Node<T>* cur_node = head_;
  do {
    if (BelongsBeforeThisValue(data, cur_node->next->data)) {
      Node<T>* temp_node = cur_node->next;
      cur_node->next = new Node<T>(data, temp_node);
      return;
    }
    cur_node = cur_node->next;
  } while (cur_node != head_);
  tail_->next = new Node<T>(data, head_);
  tail_ = tail_->next;
}

template <typename T>
bool CircularLinkedList<T>::BelongsBeforeThisValue(
    T data, T value_to_compare_with) const {
  if (node_order_ == Order::kASC) {
    return data < value_to_compare_with;
  } else {
    return data > value_to_compare_with;
  }
}

template <typename T>
void CircularLinkedList<T>::Reverse() {
  if (node_order_ == Order::kASC) {
    node_order_ = Order::kDESC;
  } else {
    node_order_ = Order::kASC;
  }
  if (head_ == nullptr) {
    return;
  }
  Node<T>* temp_head = head_;
  Node<T>* temp_tail = tail_;

  Node<T>* prev_node = tail_;
  Node<T>* cur_node = head_;
  Node<T>* next_node = nullptr;
  do {
    next_node = cur_node->next;
    cur_node->next = prev_node;
    prev_node = cur_node;
    cur_node = next_node;
  } while (next_node != head_);
  head_ = temp_tail;
  tail_ = temp_head;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const CircularLinkedList<T>& cll) {
  Node<T>* iter = cll.head_;
  // empty list condition
  if (iter == nullptr) {
    os << "Empty list";
    return os;
  }
  // non-empty list condition
  do {
    os << iter->data << '\t';
    iter = iter->next;
  } while (iter != cll.head_);

  return os;
}

#endif