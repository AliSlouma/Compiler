//
// Created by Ali on 5/2/2021.
//

#ifndef COMPILER_NFA_H
#define COMPILER_NFA_H
#include <iostream>

#include "bits/stdc++.h"
#include "Node.h"
#include "Node.cpp"
using namespace std;

class NFA {
public:

    Node* generatePattern( vector<char>  &vec, string string1);
    Node* generateGraph(vector<Node*> &vec);
    int counter=0;

private:

    Node* mergeOR(Node *start , Node *start2);
    Node* mergeAND(Node *start , Node *start2);
    Node* mergeStar(Node *start );
    Node* mergePlus(Node *start);
    Node* generateTwoNodes(char c);



};


#endif //COMPILER_NFA_H
