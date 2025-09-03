#include <iostream>

#include "student.hpp"

int main() {
  Student bob = Student("Bob", "12345", "bob1");
  Student st("Ike South", "000000000", "ike");
  std::cout << st;
  return 0;
}
