#include <iomanip>
#include <sstream>
#include <stdexcept>
using std::endl;
using std::ostream;
using std::ostringstream;
using std::setfill;
using std::setw;
using std::string;

#include "doubly_linked_list.hpp"

DoublyLinkedList::DoublyLinkedList(char val) {
  head_ = new Node(val);
  tail_ = head_;
  size_ = 1;
}

DoublyLinkedList::DoublyLinkedList(const std::vector<char>& values) {
  if (values.empty()) {
    return;
  }
  size_ = values.size();
  Node* cur_node = new Node(values[0]);
  head_ = cur_node;
  for (size_t i = 1; i < size_; i++) {
    cur_node->next = new Node(values[i]);
    cur_node->next->prev = cur_node;
    cur_node = cur_node->next;
  }
  tail_ = cur_node;
}

DoublyLinkedList::DoublyLinkedList(const DoublyLinkedList& rhs) {
  if (rhs.size_ == 0) {
    return;
  }
  size_ = rhs.size_;
  Node* cur_node = new Node(rhs.head_->data);
  head_ = cur_node;
  Node* cur_rhs = rhs.head_;
  for (size_t i = 1; i < size_; i++) {
    cur_rhs = cur_rhs->next;
    cur_node->next = new Node(cur_rhs->data);
    cur_node->next->prev = cur_node;
    cur_node = cur_node->next;
  }
  tail_ = cur_node;
}

DoublyLinkedList& DoublyLinkedList::operator=(const DoublyLinkedList& rhs) {
  if (this == &rhs) {
    return *this;
  }
  Clear();
  if (rhs.size_ == 0) {
    return *this;
  }
  size_ = rhs.size_;
  Node* cur_node = new Node(rhs.head_->data);
  head_ = cur_node;
  Node* cur_rhs = rhs.head_;
  for (size_t i = 1; i < size_; i++) {
    cur_rhs = cur_rhs->next;
    cur_node->next = new Node(cur_rhs->data);
    cur_node->next->prev = cur_node;
    cur_node = cur_node->next;
  }
  tail_ = cur_node;
  return *this;
}

DoublyLinkedList::~DoublyLinkedList() { Clear(); }

char& DoublyLinkedList::Back() {
  if (tail_ == nullptr) {
    throw std::exception();
  }
  return tail_->data;
}
char& DoublyLinkedList::Front() {
  if (head_ == nullptr) {
    throw std::exception();
  }
  return head_->data;
}
void DoublyLinkedList::PopBack() {
  if (tail_ == nullptr) {
    throw std::exception();
  }
  Node* cur_node = tail_;
  if (size_ == 1) {
    head_ = nullptr;
    tail_ = nullptr;
  } else {
    tail_->prev->next = nullptr;
    tail_ = tail_->prev;
  }
  size_--;
  delete cur_node;
}
void DoublyLinkedList::PopFront() {
  if (head_ == nullptr) {
    throw std::exception();
  }
  Node* cur_node = head_;
  if (size_ == 1) {
    head_ = nullptr;
    tail_ = nullptr;
  } else {
    head_->next->prev = nullptr;
    head_ = head_->next;
  }
  size_--;
  delete cur_node;
}

void DoublyLinkedList::PushBack(char val) {
  if (size_ == 0) {
    head_ = new Node(val);
    tail_ = head_;
    size_ = 1;
  } else {
    Node* cur_node = tail_;
    cur_node->next = new Node(val);
    cur_node->next->prev = cur_node;
    tail_ = cur_node->next;
    size_++;
  }
}
void DoublyLinkedList::PushFront(char val) {
  if (size_ == 0) {
    head_ = new Node(val);
    tail_ = head_;
    size_ = 1;
  } else {
    Node* cur_node = head_;
    cur_node->prev = new Node(val);
    cur_node->prev->next = cur_node;
    head_ = cur_node->prev;
    size_++;
  }
}

void DoublyLinkedList::InsertAt(size_t idx, char val) {
  if (idx == 0) {
    PushFront(val);
    return;
  }
  if (idx > size_) {
    throw std::exception();
  }
  if (idx == size_) {
    PushBack(val);
    return;
  }
  Node* left_node = head_;
  for (size_t i = 1; i < idx; i++) {
    left_node = left_node->next;
  }
  Node* right_node = left_node->next;
  Node* new_node = new Node(val);
  left_node->next = new_node;
  new_node->prev = left_node;
  new_node->next = right_node;
  right_node->prev = new_node;
  size_++;
}
void DoublyLinkedList::EraseAt(size_t idx) {
  if (idx >= size_) {
    throw std::exception();
  }
  if (size_ == 0) {
    throw std::exception();
  }
  if (idx == 0) {
    PopFront();
    return;
  }
  if (idx == size_ - 1) {
    PopBack();
    return;
  }
  Node* index_node = head_;
  for (size_t i = 0; i < idx; i++) {
    index_node = index_node->next;
  }
  index_node->prev->next = index_node->next;
  index_node->next->prev = index_node->prev;
  size_--;
  delete index_node;
}

char& DoublyLinkedList::Find(char val) {
  if (size_ == 0) {
    throw std::exception();
  }
  Node* cur_node = head_;
  for (size_t i = 1; i < size_; i++) {
    if (cur_node->data == val) {
      return cur_node->data;
    }
    cur_node = cur_node->next;
  }
  if (cur_node->data == val) {
    return cur_node->data;
  } else {
    throw std::exception();
  }
}
void DoublyLinkedList::Clear() {
  if (size_ == 0) {
    return;
  }
  Node* cur_node = head_;
  for (size_t i = 1; i < size_; i++) {
    cur_node = cur_node->next;
    delete cur_node->prev;
  }
  delete cur_node;
  head_ = nullptr;
  tail_ = nullptr;
  size_ = 0;
}

// Functions provided to you
ostream& operator<<(ostream& os, const DoublyLinkedList& ll) {
  constexpr unsigned int kWidth = 16;
  os << "Doubly Linked List" << endl;
  os << "Head: " << ll.head_ << "\t Tail: " << ll.tail_ << endl;
  os << "Size: " << ll.size_ << endl;
  if (ll.head_ == nullptr) return (os << "Empty" << endl);
  const Node* curr = ll.head_;
  os << "Nodes (accessible from head to tail):" << endl << endl;
  os << setw(kWidth) << setfill(' ') << ' ' << ' ' << Center("prev", kWidth - 1)
     << ' ' << Center("data", kWidth - 1) << ' ' << Center("next", kWidth - 1)
     << endl;
  while (curr != nullptr) {
    ostringstream oss;
    oss << (curr->prev);
    string prev = oss.str();
    oss.str("");
    oss << (curr->next);
    string next = oss.str();
    oss.str("");
    oss << (curr->data);
    string data = oss.str();
    oss.str("");
    oss << curr;
    string address = oss.str();
    os << setw(kWidth) << setfill(' ') << ' ' << '.' << setw(kWidth)
       << setfill('-') << '.' << setw(kWidth) << '.' << setw(kWidth) << '.'
       << endl;
    os << setw(kWidth) << setfill(' ') << Center(address, kWidth - 1) << '|'
       << setw(kWidth - 1) << setfill(' ') << Center(prev, kWidth - 1) << '|'
       << setw(kWidth - 1) << Center(data, kWidth - 1) << '|'
       << setw(kWidth - 1) << Center(next, kWidth - 1) << '|' << endl;
    os << setw(kWidth) << setfill(' ') << ' ' << '\'' << setw(kWidth)
       << setfill('-') << '\'' << setw(kWidth) << '\'' << setw(kWidth) << '\''
       << endl;
    os << endl;
    curr = curr->next;
  }
  return os;
}

std::string Center(const std::string& str, decltype(str.length()) col_width) {
  // quick and easy (but error-prone) implementation
  decltype(str.length()) padl = (col_width - str.length()) / 2;
  decltype(str.length()) padr = (col_width - str.length()) - padl;
  string strf = string(padl, ' ') + str + string(padr, ' ');
  return strf;
}