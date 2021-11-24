//
// Created by Ali on 5/2/2021.
//

#include "NFA.h"
#include <iostream>
#include "bits/stdc++.h"

using namespace std;
Node *NFA::generateGraph(vector<Node*> &vec) {
    Node* start = new Node(counter++,false);
    for(int i=0;i<vec.size();i++){
        start->children['\0'].push_back(vec[i]);
    }
    return start;
}


Node *NFA::generatePattern(vector<char> &vec ,string string1) {

    stack<Node*> stack;
    Node* tmp;
    for(int i=0;i<vec.size();i++) {

        switch (vec[i]) {
            case '|':
                tmp = stack.top();
                stack.pop();
                tmp = mergeOR(stack.top(),tmp);
                stack.pop();
                stack.push(tmp);
                break;

            case '&':
                tmp = stack.top();
                stack.pop();
                tmp = mergeAND(stack.top(),tmp);
                stack.pop();
                stack.push(tmp);
                break;

            case '$':
                tmp = mergeStar(stack.top());
                stack.pop();
                stack.push(tmp);
                break;
            case '@':
                tmp = mergePlus(stack.top());
                stack.pop();
                stack.push(tmp);
                break;


            default:
                    stack.push(generateTwoNodes(vec[i]));
                break;
        }
    }

    tmp = stack.top();
    stack.pop();
    tmp->end->acceptingDefS.push_back(string1);

    return tmp;


}

Node *NFA::generateTwoNodes(char c)  {

    Node *start = new Node(counter++,false);
    Node *end = new Node(counter++,true);

    start->children[c].push_back(end);

    start->end = end;

    return start;
}


Node* NFA::mergeOR(Node *start, Node *start2) {
    Node *newStart = new Node(counter++, false);
    Node *newEnd = new Node(counter++, true);

    newStart->end = newEnd;

    start->end->accepting = false;
    start2->end->accepting = false;


    newStart->children['\0'].push_back(start);
    newStart->children['\0'].push_back(start2);

    start->end->children['\0'].push_back(newEnd);
    start2->end->children['\0'].push_back(newEnd);

    return newStart;

}

Node *NFA::mergeAND(Node *start, Node *start2) {
    start->end->accepting = false;

    start->end->children['\0'].push_back(start2);


    start->end = start2->end;

    return start;
}

Node *NFA::mergeStar(Node *start) {
    Node *newStart = new Node(counter++, false);
    Node *newEnd = new Node(counter++, true);

    newStart->end = newEnd; // set the new end

    newStart->children['\0'].push_back(newEnd);
    newStart->children['\0'].push_back(start);


    start->end->accepting = false;

    start->end->children['\0'].push_back(start);
    start->end->children['\0'].push_back(newEnd);


    return newStart;

}

Node *NFA::mergePlus(Node *start) {

    Node *newStart = new Node(counter++, false);
    Node *newEnd = new Node(counter++, true);

    newStart->end = newEnd; // set the new end

    newStart->children['\0'].push_back(start);


    start->end->accepting = false;

    start->end->children['\0'].push_back(start);
    start->end->children['\0'].push_back(newEnd);


    return newStart;
}
