#include <iostream>

#include "cs225/textfile.h"

#include "lphashtable.h"
#include "schashtable.h"
#include "dhhashtable.h"

using namespace std;

int main()
{
    std::cout << "This main file is for you to write and run your own test cases." << std::endl;
    std::cout << "To run the provided test cases, run `./test`" << std::endl;
    // feel free to insert manual test cases here

    TextFile infile("../tests/textLong.txt");
	LPHashTable<string, int> hashTable(16);
	int val = 0;
	while (infile.good())
	{
		string word = infile.getNextWord();
		if (!hashTable.keyExists(word))
		{
			val++;
			hashTable.insert(word, val);
		}
	}
	std::cout << hashTable.tableSize() << " >= " << val / 0.7 << std::endl;
	
    return 0;
}
