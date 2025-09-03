#include <iostream>

#include "circular-linked-list.hpp"

int main() {
  CircularLinkedList<int> list = CircularLinkedList<int>();

  CircularLinkedList<int> new_list = list;
  list.InsertInOrder(1);
  list.InsertInOrder(7);
  list.InsertInOrder(2);
  list.InsertInOrder(5);
  list.InsertInOrder(3);
  new_list.Reverse();
  list.Reverse();
  list.InsertInOrder(4);
  list.InsertInOrder(6);
  new_list.InsertInOrder(6);
  new_list.InsertInOrder(4);
  new_list = list;
  list.~CircularLinkedList();
  std::cout << list << std::endl;
  std::cout << new_list << std::endl;
}
