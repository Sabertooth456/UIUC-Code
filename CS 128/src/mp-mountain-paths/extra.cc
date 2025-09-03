
#include <cstring>
#include <iostream>
#include <vector>

std::vector<char*> GetNames(std::istream& input) {
  std::vector<char*> ptr_vec;
  char temp_arr[100];
  input >> temp_arr;
  while (input.good()) {
    size_t allocate_len = strlen(temp_arr) + 1;
    char* new_arr[]{new char[allocate_len]};
    strcpy(*new_arr, temp_arr);
    ptr_vec.push_back(*new_arr);
    input >> temp_arr;
  }
  return ptr_vec;
}

// Initialize: ptr_vec, temp_arr[100];
// read_status (read a name from input to temp_arr)
// while read_status is successful:
//   Initialize allocate_len (name_length + 1)
//   Initialize new_arr (allocate a char array in the free store)
//   Copy the name in temp_arr to new_arr
//   Push back new_arr to ptr_vector
//   Update read_status (read another name, and check status)
// return ptr_vec