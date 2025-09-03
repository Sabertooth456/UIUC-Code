#include "functions.hpp"

Person Process(std::string line) {
  std::vector<std::string> processed_line = utilities::GetSubstrs(line, ',');
  std::string name = processed_line[0];
  processed_line.erase(processed_line.begin());
  std::vector<std::string> sequence_to_check_amount = processed_line;
  Person person;
  person.name = name;
  for (std::string amount : sequence_to_check_amount) {
    person.sequence_count.push_back(std::stoi(amount));
  }
  return person;
}

std::vector<std::string> ProcessFirstLine(std::string first_line) {
  std::vector<std::string> line_data = utilities::GetSubstrs(first_line, ',');
  line_data.erase(line_data.begin());
  return line_data;
}

int HighestConsecutiveRepitition(std::string sequence_to_check,
                                 std::string entire_dna_sequence) {
  int max_count = 0;
  int cur_count = 0;
  for (size_t i = 0;
       i < entire_dna_sequence.length() - sequence_to_check.length() + 2; i++) {
    size_t j = 0;
    for (; j < sequence_to_check.length(); ++j) {
      if (entire_dna_sequence[i + j] != sequence_to_check[j]) {
        break;
      }
    }
    if (j == sequence_to_check.length()) {  // "AGATGXXXXXAGATGAGATG"
      cur_count++;
      if (cur_count > max_count) {
        max_count = cur_count;
      }
      i += sequence_to_check.length() - 1;
    } else {
      cur_count = 0;
    }
  }
  return max_count;
}

bool MatchPerson(Person person, std::vector<int> relevant_sequence_counts) {
  size_t i = 0;
  for (; i < relevant_sequence_counts.size(); i++) {
    if (person.sequence_count[i] != relevant_sequence_counts[i]) {
      return false;
    }
  }
  return true;
}

std::vector<int> CountRelevantSequences(
    std::vector<std::string> relevant_sequences, std::string dna_sequence) {
  std::vector<int> relevant_sequence_counts;
  relevant_sequence_counts.reserve(relevant_sequences.size());
  for (std::string sequence_to_check : relevant_sequences) {
    relevant_sequence_counts.push_back(
        HighestConsecutiveRepitition(sequence_to_check, dna_sequence));
  }
  return relevant_sequence_counts;
}