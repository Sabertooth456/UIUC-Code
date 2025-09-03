/**
 * @file lphashtable.cpp
 * Implementation of the LPHashTable class.
 */
#include "lphashtable.h"

template <class K, class V>
LPHashTable<K, V>::LPHashTable(size_t tsize)
{
    if (tsize <= 0)
        tsize = 17;
    size = findPrime(tsize);
    table = new std::pair<K, V>*[size];
    should_probe = new bool[size];
    for (size_t i = 0; i < size; i++) {
        table[i] = NULL;
        should_probe[i] = false;
    }
    elems = 0;
}

template <class K, class V>
LPHashTable<K, V>::~LPHashTable()
{
    for (size_t i = 0; i < size; i++)
        delete table[i];
    delete[] table;
    delete[] should_probe;
}

template <class K, class V>
LPHashTable<K, V> const& LPHashTable<K, V>::operator=(LPHashTable const& rhs)
{
    if (this != &rhs) {
        for (size_t i = 0; i < size; i++)
            delete table[i];
        delete[] table;
        delete[] should_probe;

        table = new std::pair<K, V>*[rhs.size];
        should_probe = new bool[rhs.size];
        for (size_t i = 0; i < rhs.size; i++) {
            should_probe[i] = rhs.should_probe[i];
            if (rhs.table[i] == NULL)
                table[i] = NULL;
            else
                table[i] = new std::pair<K, V>(*(rhs.table[i]));
        }
        size = rhs.size;
        elems = rhs.elems;
    }
    return *this;
}

template <class K, class V>
LPHashTable<K, V>::LPHashTable(LPHashTable<K, V> const& other)
{
    table = new std::pair<K, V>*[other.size];
    should_probe = new bool[other.size];
    for (size_t i = 0; i < other.size; i++) {
        should_probe[i] = other.should_probe[i];
        if (other.table[i] == NULL)
            table[i] = NULL;
        else
            table[i] = new std::pair<K, V>(*(other.table[i]));
    }
    size = other.size;
    elems = other.elems;
}

template <class K, class V>
void LPHashTable<K, V>::insert(K const& key, V const& value)
{

    /**
     * @todo Implement this function.
     *
     * @note Remember to resize the table when necessary (load factor >= 0.7).
     * **Do this check *after* increasing elems (but before inserting)!!**
     * Also, don't forget to mark the cell for probing with should_probe!
     */
    // remove(key);
    unsigned int address = hashes::hash(key, size);
    while(table[address % size] != nullptr) {
        should_probe[address % size] = true;
        address++;
    }
    table[address % size] = new std::pair<K, V>(key, value);
    elems++;
    if ((1.0 * elems) / size >= 0.7) {
        resizeTable();
    }
}

template <class K, class V>
void LPHashTable<K, V>::remove(K const& key)
{
    /**
     * @todo: implement this function
     */
    unsigned int address = hashes::hash(key, size);
    while(table[address % size] != nullptr || should_probe[address % size]) {
        // std::cout << address << " : " << table[address]->first << " " << table[address]->second << std::endl;
        if (table[address % size] != nullptr && table[address % size]->first == key) {
            delete table[address % size];
            table[address % size] = nullptr;
            elems--;
            break;
        }
        address++;
    }
}

template <class K, class V>
int LPHashTable<K, V>::findIndex(const K& key) const
{
    
    /**
     * @todo Implement this function
     *
     * Be careful in determining when the key is not in the table!
     */
    unsigned int address = hashes::hash(key, size);
    while(table[address % size] != nullptr || should_probe[address % size]) {
        std::pair<K, V>* p = table[address % size];
        if (table[address % size] != nullptr && table[address % size]->first == key) {
            return address;
        }
        address++;
    }
    if (table[address % size] != nullptr && table[address % size]->first == key) {
        return address;
    }
    return -1;
}

template <class K, class V>
V LPHashTable<K, V>::find(K const& key) const
{
    int idx = findIndex(key);
    if (idx != -1)
        return table[idx]->second;
    return V();
}

template <class K, class V>
V& LPHashTable<K, V>::operator[](K const& key)
{
    // First, attempt to find the key and return its value by reference
    int idx = findIndex(key);
    if (idx == -1) {
        // otherwise, insert the default value and return it
        insert(key, V());
        idx = findIndex(key);
    }
    return table[idx]->second;
}

template <class K, class V>
bool LPHashTable<K, V>::keyExists(K const& key) const
{
    return findIndex(key) != -1;
}

template <class K, class V>
void LPHashTable<K, V>::clear()
{
    for (size_t i = 0; i < size; i++)
        delete table[i];
    delete[] table;
    delete[] should_probe;
    table = new std::pair<K, V>*[17];
    should_probe = new bool[17];
    for (size_t i = 0; i < 17; i++)
        should_probe[i] = false;
    size = 17;
    elems = 0;
}

template <class K, class V>
void LPHashTable<K, V>::resizeTable()
{

    /**
     * @todo Implement this function
     *
     * The size of the table should be the closest prime to size * 2.
     *
     * @hint Use findPrime()!
     */
    
    size_t oldSize = size;
    std::pair<K, V> **oldTable = table;
    elems = 0;
    size = findPrime(oldSize * 2);
    table = new std::pair<K, V>*[size];
    delete[] should_probe;
    should_probe = new bool[size];
    for (size_t idx = 0; idx < size; idx++) {
        table[idx] = nullptr;
        should_probe[idx] = false;
    }
    for (size_t idx = 0; idx < oldSize; idx++) {
        if (oldTable[idx] != nullptr) {
            std::pair<K, V>* p = oldTable[idx];
            K first = p->first;
            V second = p->second;
            insert(p->first, p->second);
            delete oldTable[idx];
            oldTable[idx] = nullptr;
        }
    }
    delete[] oldTable;
}
