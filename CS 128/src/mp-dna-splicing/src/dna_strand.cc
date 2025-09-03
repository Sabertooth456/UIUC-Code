#include "dna_strand.hpp"

#include <stdexcept>

DNAstrand::DNAstrand(Node* head, Node* tail) {
  head_ = head;
  tail_ = tail;
}

// DNAstrand::DNAstrand(const char* sequence) {
//   head_ = nullptr;
//   tail_ = nullptr;
//   Node* cur = nullptr;
//   for (unsigned int i = 0; sequence[i] != '\0'; i++) {
//     if (i == 0) {
//       head_ = new Node(sequence[i]);
//       cur = head_;
//     } else {
//       cur->next = new Node(sequence[i]);
//       cur = cur->next;
//     }
//   }
//   tail_ = cur;
// }

Node* DNAstrand::GetHead() const { return head_; }
Node* DNAstrand::GetTail() const { return tail_; }

DNAstrand::~DNAstrand() {
  Node* cur = head_;
  head_ = nullptr;
  tail_ = nullptr;
  while (cur != nullptr) {
    Node* next = cur->next;
    delete cur;
    cur = next;
  }
}

Node* DNAstrand::Snip(Node* node, unsigned int length) {
  Node* cur = node;
  for (unsigned int i = 0; i < length; i++) {
    Node* next = cur->next;
    delete cur;
    cur = next;
  }
  return cur;
}

void DNAstrand::SpliceIn(const char* pattern, DNAstrand& to_splice_in) {
  unsigned int pattern_length = 0;
  for (; pattern[pattern_length] != '\0'; pattern_length++)
    ;
  Node* node_before_latest_match = Search(pattern);
  if (node_before_latest_match == nullptr) {
    throw std::exception();
  }
  Node* latest_match = nullptr;
  if (node_before_latest_match == head_) {
    latest_match = node_before_latest_match;
    node_before_latest_match = nullptr;
  } else {
    latest_match = node_before_latest_match->next;
  }
  if (node_before_latest_match == nullptr) {
    head_ = Snip(latest_match, pattern_length);
    if (head_ == nullptr) {
      head_ = to_splice_in.head_;
      tail_ = to_splice_in.tail_;
    } else {
      to_splice_in.tail_->next = head_;
      head_ = to_splice_in.head_;
    }
  } else {
    to_splice_in.tail_->next = Snip(latest_match, pattern_length);
    node_before_latest_match->next = to_splice_in.head_;
    if (to_splice_in.tail_->next == nullptr) {
      tail_ = to_splice_in.tail_;
    }
  }
  to_splice_in.head_ = nullptr;
  to_splice_in.tail_ = nullptr;
}

Node* DNAstrand::Search(const char* pattern) {
  unsigned int pattern_length = 0;
  for (; pattern[pattern_length] != '\0'; pattern_length++)
    ;
  Node* prev = nullptr;
  Node* cur = head_;
  Node* node_before_latest_match = nullptr;
  Node* latest_match = nullptr;
  bool reached_end = false;
  while (!reached_end && cur != nullptr) {
    Node* to_check = cur;
    bool match_found = true;
    for (unsigned int i = 0; i < pattern_length; i++) {
      if (to_check == nullptr) {
        match_found = false;
        reached_end = true;
        break;
      }
      if (to_check->data != pattern[i]) {
        match_found = false;
        continue;
      }
      to_check = to_check->next;
    }
    if (match_found) {
      node_before_latest_match = prev;
      latest_match = cur;
    }
    prev = cur;
    cur = cur->next;
  }
  if (node_before_latest_match == nullptr) {
    return latest_match;
  } else {
    return node_before_latest_match;
  }
}