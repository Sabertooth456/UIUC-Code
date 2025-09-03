#include <cctype>
#include <iostream>
#include <string>

#include "functions.hpp"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: ./bin/exec word" << std::endl;
    return 1;
  }

  std::string pronouced_words;

  std::string phrase = argv[1];

  std::string word;

  for (size_t i = 0; i < phrase.length(); i++) {
    char c = phrase[i];
    if (c != ' ') {
      word.push_back(tolower(phrase[i]));
    } else {
      std::string phonetics = Pronounce(word);
      pronouced_words += phonetics + " ";
      word = "";
    }
  }
  if (!word.empty()) {
    pronouced_words += Pronounce(word);
  }
  std::cout << pronouced_words << std::endl;
}

/* Gameplan

Separate the words (Done)

Translate each word, according to the CS128 pronounciations

*/