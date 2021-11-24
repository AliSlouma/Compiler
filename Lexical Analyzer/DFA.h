//
// Created by omara on 5/3/2021.
//

#ifndef COMPILER_DFA_H
#define COMPILER_DFA_H

#include "Node.h"

typedef std::set<Node *> nset;

class DFA {
private :
    void epsilonCrawlerV3( Node *start);
    void checkInput(const nset &startSet, char input, nset &newSet);

    void checkInputs(char start, char end, map<nset, pair<int, vector<pair<char, int>>>> &theAllSet,
                     nset &tempSet,
                     vector<nset> &newSets, int &n);

    static Node *makeNewGraph(int &n, map<nset, pair<int, vector<pair<char, int>>>> &theAllSet);
    void getAllNFANodesEpsilons(nset &startNodes) ;
    void dfs(Node * currNode) ;
    nset allNFANodes ;
    map<Node *,nset > nodeToEpsilonSet ;
public :
    Node *NFA_To_Dfa(nset &startNodes);
};

#endif //COMPILER_DFA_H
