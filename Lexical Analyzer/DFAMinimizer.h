
#ifndef COMPILER_DFAMINIMIZER_H
#define COMPILER_DFAMINIMIZER_H

#include "Node.h"
#include "Node.cpp"
#include "parser_data.h"

class DFAMinimizer {
private:
    int cnt;
    vector<int> type;
    int startID;
    map<int, Node *> nodes;
    Node *start;
    map<string,int> priorities;

    void obtainNodes(Node *node);

    vector<vector<Node *>> partition();

    void updateType(vector<vector<Node *>> &v);

    Node *buildMinimized(vector<vector<Node *>> &groups);

    vector<vector<struct Node *>> initPartitions();

    void freeDFA();

public:
    DFAMinimizer(Node *start, parser_data *data);

    Node *minimizeGraph();
};

#endif //COMPILER_DFAMINIMIZER_H
