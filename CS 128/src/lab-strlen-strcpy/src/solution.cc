
#include "solution.hpp"

unsigned int StrLen(const char* c_str) {
  unsigned int counter = 0;
  unsigned int i = 0;
  while (c_str[i] != '\0') {
    counter++;
    i++;
  }
  return i;
}

void StrCpy(char*& to, const char* from) {
  if (from == nullptr) {
    return;
  }
  int length = StrLen(from);
  char* copy = new char[StrLen(from) + 1];
  for (int i = 0; i < length; i++) {
    copy[i] = from[i];
  }
  copy[length] = '\0';
  delete[] to;
  to = copy;
}