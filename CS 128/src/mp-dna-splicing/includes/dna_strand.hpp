#ifndef DNA_STRAND_HPP
#define DNA_STRAND_HPP

#include "node.hpp"

class DNAstrand {
 public:
  DNAstrand() = default;
  DNAstrand(Node* head, Node* tail);
  DNAstrand(const DNAstrand& rhs) = delete;
  DNAstrand& operator=(const DNAstrand& rhs) = delete;
  Node* GetHead() const;
  Node* GetTail() const;
  Node* Snip(Node* node, unsigned int length);
  Node* Search(const char* pattern);
  void SpliceIn(const char* pattern, DNAstrand& to_splice_in);
  ~DNAstrand();

 private:
  Node* head_ = nullptr;
  Node* tail_ = nullptr;
};

#endif