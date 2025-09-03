/**
 * @file cartalk_puzzle.cpp
 * Holds the function which solves a CarTalk puzzler.
 *
 * @author Matt Joras
 * @date Winter 2013
 */

#include <fstream>

#include "cartalk_puzzle.h"

using namespace std;

/**
 * Solves the CarTalk puzzler described here:
 * http://www.cartalk.com/content/wordplay-anyone.
 * @return A vector of (string, string, string) tuples
 * Returns an empty vector if no solutions are found.
 * @param d The PronounceDict to be used to solve the puzzle.
 * @param word_list_fname The filename of the word list to be used.
 */
vector<std::tuple<std::string, std::string, std::string>> cartalk_puzzle(PronounceDict d,
                                    const string& word_list_fname)
{
    vector<std::tuple<std::string, std::string, std::string>> ret;

    /* Your code goes here! */
    ifstream wordlistfile(word_list_fname);
    string word;
    if (wordlistfile.is_open()) {
      while (getline(wordlistfile, word)) {
          string s1 = word.substr(1);
          string s2;
          if(word.length() > 2){
            s2 = word.front() + word.substr(2);
          }
          else{
            s2 = word.substr(0, 1);
          }

          if(d.homophones(word, s1) && d.homophones(word, s2)){
            ret.push_back({word, s1, s2});
          }
      }
    }
    return ret;
}
