#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <string>
#include <vector>

// Your function declarations should go in this header file.

std::string Pronounce(std::string word);

std::string FirstSegmentAt(size_t index, std::string string);

std::string FindSegment(std::string string);

std::string FindWSegment(std::string string);

std::string SoundOf(std::string string);

bool IsVowel(std::string segment);

#endif