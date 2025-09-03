#include <iostream>

#include "dna_strand.hpp"

int main() {
  const char* sequence = "ctgaattcg";
  DNAstrand original_strand = DNAstrand(sequence);

  DNAstrand inserted_strand = DNAstrand("tgatc");
  original_strand.SpliceIn("gaattc", inserted_strand);

  Node* cur = original_strand.GetHead();
  while (cur != nullptr) {
    std::cout << cur->data;
    cur = cur->next;
  }
  std::cout << std::endl;
}