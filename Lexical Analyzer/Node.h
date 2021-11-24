//
// Created by Ali on 5/2/2021.
//

#ifndef COMPILER_NODE_H
#define COMPILER_NODE_H

#include <iostream>

#include "bits/stdc++.h"

using namespace std;

class Node {
public:

    Node(int x, bool accept) {
        id = x;
        accepting = accept;
    }

    Node(int x) {
        id = x;
        accepting = false;
    }

    int id;
    Node *end;
    bool accepting; // true for accepting nodes , false otherwise
    unordered_map<char, vector<Node *>> children;
    vector<string> acceptingDefS;

};


#endif //COMPILER_NODE_H
