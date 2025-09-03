#ifndef WORDSEARCHSOLVER_HPP
#define WORDSEARCHSOLVER_HPP

#include <cstddef>
#include <string>
#include <vector>

#include "WordLocation.hpp"

struct DirectionVector {
  size_t y_pos;
  size_t x_pos;
  int y_movement;
  int x_movement;
};

enum class CheckDirection { kHorizontal, kVertical, kLeftDiag, kRightDiag };

class WordSearchSolver {
 public:
  WordSearchSolver(const std::vector<std::vector<char>>& puzzle);
  WordLocation FindWord(const std::string& word);
  WordLocation FindWord(const std::string& word, CheckDirection direction);

 private:
  bool LocationInBounds(size_t row, size_t col) const;
  WordLocation FindWord(const std::string& word, DirectionVector vector);
  WordLocation Scan(const std::string& word, DirectionVector vector);
  std::vector<std::vector<char>> puzzle_;
  size_t puzzle_height_;
  size_t puzzle_width_;
};

#endif