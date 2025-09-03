#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <string>
#include <vector>

#include "utilities.hpp"

struct Person {
  std::string name;
  std::vector<int> sequence_count;
};

Person Process(std::string first_line);

std::vector<std::string> ProcessFirstLine(std::string first_line);

int HighestConsecutiveRepitition(std::string sequence_to_check,
                                 std::string entire_dna_sequence);

bool MatchPerson(Person person, std::vector<int> sequence_counts);

std::vector<int> CountRelevantSequences(
    std::vector<std::string> relevant_sequences, std::string dna_sequence);

#endif