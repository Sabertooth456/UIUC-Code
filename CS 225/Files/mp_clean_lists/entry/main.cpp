#include "List.h"

int main() {
  List<std::string> list;

  list.insertBack("c");
  list.insertBack("b");
  list.insertBack("a");

  list.sort();

  list.print();
  std::cout << std::endl;
  return 0;
}
