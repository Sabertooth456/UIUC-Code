#include <fstream>
#include <iostream>
#include <vector>

#include "functions.hpp"
#include "utilities.hpp"

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " [input_file] [dna_sequence]"
              << std::endl;
    return 1;
  }
  std::ifstream ifs{argv[1]};
  std::string line;
  std::getline(ifs, line);
  std::vector<std::string> relevant_sequences = ProcessFirstLine(line);
  std::vector<Person> database;
  for (std::string line; std::getline(ifs, line); line = "") {
    Person person = Process(line);
    database.push_back(person);
  }
  std::string dna_sequence = argv[2];
  std::vector<int> relevant_sequence_counts =
      CountRelevantSequences(relevant_sequences, dna_sequence);
  bool match_found = false;
  std::string match_name;
  for (Person person : database) {
    if (MatchPerson(person, relevant_sequence_counts)) {
      if (match_found) {
        match_found = false;
        break;
      } else {
        match_found = true;
        match_name = person.name;
      }
    }
  }
  if (match_found) {
    std::cout << match_name << std::endl;
  } else {
    std::cout << "No match" << std::endl;
  }
  return 0;
}