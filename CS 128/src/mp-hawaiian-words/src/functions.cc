#include "functions.hpp"
// Your function definitions should go in this source file.

std::vector<std::string> valid_segments = {
    // Two letter segments
    "ai", "ae", "ao", "au", "ei", "eu", "iu", "oi", "ou", "ui",

    // Single letter segments
    "a", "e", "i", "o", "u", "p", "k", "h", "l", "m", "n",

    // Extra cases
    "'", "\n", "\r", "\""

};

std::vector<std::string> valid_w_segments = {
    "aw", "iw", "ew", "uw", "ow",
};

std::vector<std::string> vowels = {"ai", "ae", "ao", "au", "ei",
                                   "eu", "iu", "oi", "ou", "ui",
                                   "a",  "e",  "i",  "o",  "u"};

std::vector<std::vector<std::string>> segment_pronounciations = {
    // Two letter segments
    {"ai", "eye"},
    {"ae", "eye"},
    {"ao", "ow"},
    {"au", "ow"},
    {"ei", "ay"},
    {"eu", "eh-oo"},
    {"iu", "ew"},
    {"oi", "oy"},
    {"ou", "ow"},
    {"ui", "ooey"},

    // W segments
    {"aw", "w"},
    {"iw", "v"},
    {"ew", "v"},
    {"uw", "w"},
    {"ow", "w"},
    {"w", "w"},

    // Single letter segments
    {"a", "ah"},
    {"e", "eh"},
    {"i", "ee"},
    {"o", "oh"},
    {"u", "oo"},
    {"p", "p"},
    {"k", "k"},
    {"h", "h"},
    {"l", "l"},
    {"m", "m"},
    {"n", "n"},

    // Extra cases
    {"'", "'"},
    {"\n", ""},
    {"\r", ""},
    {"\"", ""}

};

std::string Pronounce(std::string word) {
  std::string pronounciation;
  size_t word_length = word.length();
  for (size_t i = 0; i < word_length; i++) {
    std::string segment = FirstSegmentAt(i, word);
    if (segment == "INVALID") {
      return word +
             " contains a character not a part of the Hawaiian language.";
    }
    if (segment.length() == 2) {
      if (segment[1] != 'w') {
        i++;
      }
    }
    std::string sound = SoundOf(segment);
    if (sound == "INVALID") {
      return word +
             " contains a character not a part of the Hawaiian language.";
    }
    if (IsVowel(segment) && i != word_length - 1 && word[i + 1] != '\'') {
      pronounciation.append(sound + "-");
    } else {
      pronounciation.append(sound);
    }
  }
  return pronounciation;
}

std::string FirstSegmentAt(size_t index, std::string string) {
  // If the first letter is a "w", it requires "special" treatment
  if (string.at(index) == 'w') {
    if (index == 0) {
      return "w";
    }
    return FindWSegment(string.substr(index - 1, 2));
  }
  // If the index is at the last character of the string, only use the last
  // character
  int length = 2;
  if (index == string.length() - 1) {
    length = 1;
  }
  return FindSegment(string.substr(index, length));
}

std::string FindSegment(std::string string) {
  for (std::string segment : valid_segments) {
    if (string == segment || string.substr(0, 1) == segment) {
      return segment;
    }
  }
  return "INVALID";
}

std::string FindWSegment(std::string string) {
  for (std::string segment : valid_w_segments) {
    if (string == segment || string.substr(0, 1) == segment) {
      return segment;
    }
  }
  return "INVALID";
}

std::string SoundOf(std::string string) {
  for (std::vector<std::string> segment : segment_pronounciations) {
    if (string == segment[0]) {
      return segment[1];
    }
  }
  return "";
}

bool IsVowel(std::string segment) {
  for (std::string v : vowels) {
    if (segment == v) {
      return true;
    }
  }
  return false;
}