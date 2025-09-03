#include "WordSearchSolver.hpp"

bool WordSearchSolver::LocationInBounds(size_t row, size_t col) const {
  return ((row < puzzle_height_) && (col < puzzle_width_));
}

// Height = [y][]
// Width = [][x]
WordSearchSolver::WordSearchSolver(
    const std::vector<std::vector<char>>& puzzle) {
  puzzle_ = puzzle;
  puzzle_height_ = puzzle.size();
  puzzle_width_ = puzzle.at(0).size();
}

WordLocation WordSearchSolver::FindWord(const std::string& word) {
  WordLocation answer;
  answer = FindWord(word, CheckDirection::kHorizontal);
  if (!answer.word.empty()) {
    return answer;
  }
  answer = FindWord(word, CheckDirection::kLeftDiag);
  if (!answer.word.empty()) {
    return answer;
  }
  answer = FindWord(word, CheckDirection::kRightDiag);
  if (!answer.word.empty()) {
    return answer;
  }
  answer = FindWord(word, CheckDirection::kVertical);
  return answer;
}
WordLocation WordSearchSolver::FindWord(const std::string& word,
                                        CheckDirection direction) {
  DirectionVector vector = {0, 0, 0, 0};
  switch (direction) {
    case CheckDirection::kHorizontal: {
      vector.x_movement = 1;
      break;
    }
    case CheckDirection::kLeftDiag: {
      vector.x_movement = -1;
      vector.y_movement = 1;
      break;
    }
    case CheckDirection::kRightDiag: {
      vector.x_movement = 1;
      vector.y_movement = 1;
      break;
    }
    case CheckDirection::kVertical: {
      vector.y_movement = 1;
      break;
    }
  }
  return FindWord(word, vector);
}

WordLocation WordSearchSolver::FindWord(const std::string& word,
                                        DirectionVector vector) {
  WordLocation answer;
  for (size_t y = vector.y_pos; y < puzzle_height_; y++) {
    for (size_t x = vector.x_pos; x < puzzle_width_; x++) {
      DirectionVector vector_direction = {y, x, vector.y_movement,
                                          vector.x_movement};
      answer = Scan(word, vector_direction);
      if (answer.word == word) {
        return answer;
      }
      vector_direction.y_movement *= -1;
      vector_direction.x_movement *= -1;
      answer = Scan(word, vector_direction);
      if (answer.word == word) {
        return answer;
      }
    }
  }
  return answer;
}

WordLocation WordSearchSolver::Scan(const std::string& word,
                                    DirectionVector vector) {
  WordLocation answer;
  int word_length = word.length();
  int i = 0;
  for (; i < word_length; i++) {
    int y_pos = vector.y_pos + vector.y_movement * i;
    int x_pos = vector.x_pos + vector.x_movement * i;
    if (!LocationInBounds(y_pos, x_pos) || puzzle_[y_pos][x_pos] != word[i]) {
      return WordLocation{};
    }
    answer.char_positions.push_back(CharPositions{
        word[i], static_cast<size_t>(y_pos), static_cast<size_t>(x_pos)});
  }
  answer.word = word;
  return answer;
}