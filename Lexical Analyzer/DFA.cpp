//
// Created by omara on 5/3/2021.
//

#include "DFA.h"

#define INPUT_MIN 1
#define INPUT_MAX 127

void DFA::epsilonCrawlerV3(Node *node) {
    nset newSet;
    if (node->children.count((char) 0) == 0) {
        newSet.insert(node);
        nodeToEpsilonSet.insert(make_pair(node, newSet));
        return;
    } else {
        vector<Node *> children = node->children[(char) 0];
        for (auto i : children) {
            if (nodeToEpsilonSet.count(i) == 0) {
                epsilonCrawlerV3(i);
            }
            newSet.insert(nodeToEpsilonSet[i].begin(), nodeToEpsilonSet[i].end());
        }
        nodeToEpsilonSet.insert(make_pair(node, newSet));
    }
}

void DFA::dfs(Node *currNode) {
    if (this->allNFANodes.count(currNode))return;
    else {
        this->allNFANodes.insert(currNode);
        for (const auto &node1:currNode->children) {
            for (auto node2 : node1.second)if (this->allNFANodes.count(node2) == 0)dfs(node2);
        }
    }
}

void DFA::getAllNFANodesEpsilons(nset &startNodes) {
    for (auto i : startNodes) {
        dfs(i);
    }
    for (auto i :this->allNFANodes) {
        nset nodeIEpsilonSet;
        nodeIEpsilonSet.insert(i);
        if(nodeToEpsilonSet.count(i)== 0)epsilonCrawlerV3(i);
    }
}

void DFA::checkInput(const nset &startSet, char input, nset &newSet) {
    for (auto i : startSet) {
        if (i->children.count(input)) {
            newSet.insert(i->children[input].begin(), i->children[input].end());
        }
    }
    nset tempSet;
    for (auto i : newSet) {
        tempSet.insert(nodeToEpsilonSet[i].begin(), nodeToEpsilonSet[i].end());
        //epsilonCrawler(newSet, i);
    }
    newSet.insert(tempSet.begin(), tempSet.end());
}

void DFA::checkInputs(char start, char end, map<nset, pair<int, vector<pair<char, int>>>> &theAllSet,
                      nset &startSet,
                      vector<nset> &newSets,
                      int &n) {
    for (char i = start; i < end; i++) {
        nset newSet;
        vector<pair<char, int>> newInputs;
        checkInput(startSet, i, newSet);
        int newSetCount = theAllSet.count(newSet);
        if (!newSet.empty() && newSetCount == 0) {
            theAllSet.insert(make_pair(newSet, make_pair(n, newInputs)));
            newSets.push_back(newSet);
            theAllSet[startSet].second.emplace_back(i, n);
            n++;
        } else if (newSet.empty()) {
            theAllSet[startSet].second.emplace_back(i, 0);
        } else {
            theAllSet[startSet].second.emplace_back(i, theAllSet[newSet].first);
        }
    }
}

Node *DFA::makeNewGraph(int &n, map<nset, pair<int, vector<pair<char, int>>>> &theAllSet) {
    map<int, Node *> newNodes;
    for (int i = 0; i <= n; i++) {
        newNodes[i] = new Node(i);
    }
    for (auto &i : theAllSet) {
        pair<int, vector<pair<char, int>>> nodeAttr = i.second;
        int nodeId = nodeAttr.first;
        Node *node = newNodes[nodeId];
        for (auto &j : nodeAttr.second) {
            int y = j.second;
            char edge = j.first;
            vector<Node *> tempVector;
            tempVector.push_back(newNodes[y]);
            node->children.insert(make_pair(edge, tempVector));
        }
        nset nodeSet = i.first;
        for (auto j : nodeSet) {
            if (j->accepting) {
                node->acceptingDefS.insert(node->acceptingDefS.end(), j->acceptingDefS.begin(),
                                           j->acceptingDefS.end());
                node->accepting = true;
            }
        }
    }
    return newNodes[1];
}

Node *DFA::NFA_To_Dfa(nset &startNodes) {
    map<nset, pair<int, vector<pair<char, int>>>> theAllSet;
    vector<nset> newSets;
    int n = 1;
    nset startSet;
    vector<pair<char, int>> inputs;
    startSet.insert(startNodes.begin(), startNodes.end());
    getAllNFANodesEpsilons(startNodes);
    for (auto i : startNodes) {
        startSet.insert(nodeToEpsilonSet[i].begin(), nodeToEpsilonSet[i].end());
    }
    newSets.push_back(startSet);
    theAllSet.insert(make_pair(startSet, make_pair(n, inputs)));
    n++;
    while (!newSets.empty()) {
        nset tempSet = newSets.back();
        newSets.pop_back();
        checkInputs((char) INPUT_MIN, (char) INPUT_MAX, theAllSet, tempSet, newSets, n);
    }
    return makeNewGraph(n, theAllSet);
}