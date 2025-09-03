#include <iostream>
#include <stdexcept>
#include <vector>

#include "doubly_linked_list.hpp"

using namespace std;

int main() {
  std::vector<char> vector = {'0', '1', '2', '3', '4', '5'};
  DoublyLinkedList test = DoublyLinkedList(vector);
  std::cout << test.Find('0') << std::endl;
  std::cout << test.Find('1') << std::endl;
  std::cout << test.Find('2') << std::endl;
  std::cout << test.Find('3') << std::endl;
  std::cout << test.Find('4') << std::endl;
  std::cout << test.Find('5') << std::endl;
  std::cout << test.Find('6') << std::endl;
}
