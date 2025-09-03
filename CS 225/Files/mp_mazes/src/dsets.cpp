/* Your code here! */
#include "dsets.h"

void DisjointSets::addelements(int num) {
    if (num < 0) {
        std::cout << "addelements() : num parameter is negative" << std::endl;
        return;
    }
    for (int n = 0; n < num; ++n) {
        nodes.push_back(-1);
    }
}

int DisjointSets::find(int elem) {
    if (!validIndex(elem)) {
        std::cout << "find() : elem parameter outside of bounds" << std::endl;
        return -1;
    }
    if (nodes[elem] < 0) {
        return elem;
    } else {
        int rootIndex = find(nodes[elem]);
        nodes[elem] = rootIndex;
        return rootIndex;
    }
}

void DisjointSets::setunion(int a, int b) {
    if (!validIndex(a) || !validIndex(b)) {
        std::cout << "setunion() : a or b parameter outside of bounds" << std::endl;
        return;
    }
    int rootIndexA = find(a);
    int rootIndexB = find(b);
    if (rootIndexA == rootIndexB) {
        return;
    }
    if (nodes[rootIndexA] > nodes[rootIndexB]) {
        nodes[rootIndexB] += nodes[rootIndexA];
        nodes[rootIndexA] = rootIndexB;
    } else {
        nodes[rootIndexA] += nodes[rootIndexB];
        nodes[rootIndexB] = rootIndexA;
    }
}

int DisjointSets::size(int elem) {
    if (!validIndex(elem)) {
        std::cout << "size() : elem parameter outside of bounds" << std::endl;
        return -1;
    }
    return -nodes[find(elem)];
}

bool DisjointSets::validIndex(int idx) {
    return (idx >= 0 && idx < int(nodes.size()));
}