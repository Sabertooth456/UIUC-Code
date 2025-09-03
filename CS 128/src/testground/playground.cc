#include "playground.hpp"

#include <fstream>
#include <stdexcept>
#include <vector>

std::vector<int> get_ints_from_file(std::string file_name) {
  std::ifstream ifs(file_name);
  if (!ifs.is_open()) {
    throw std::exception();
  }
  std::vector<int> vector_of_ints;
  int value = 0;
  while (ifs.good()) {
    ifs >> value;
    if (ifs.fail()) {
      ifs.clear();
      ifs.ignore(1, '\n');
    } else {
      vector_of_ints.push_back(value);
    }
  }
  return vector_of_ints;
}