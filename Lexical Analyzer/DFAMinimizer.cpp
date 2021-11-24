#include "DFAMinimizer.h"

const int ASCII = 127;

DFAMinimizer::DFAMinimizer(Node *start, parser_data *data) {
    this->start = start;
    startID = start->id;
    obtainNodes(start);
    cnt = nodes.size();
    this->priorities = data->get_priority();
}

Node *DFAMinimizer::minimizeGraph() {
    for (int i = 0; i < ASCII; i++) {
        nodes[0]->children[i].push_back(nodes[0]);
    }
    vector<vector<Node *>> groups = partition();
    Node *newStart = buildMinimized(groups);
    freeDFA();
    return newStart;
}


void DFAMinimizer::obtainNodes(Node *node) {
    nodes[node->id] = node;
    for (auto &input_vec: node->children) {
        for (auto &nxt: input_vec.second) {
            if (nodes.find(nxt->id) == nodes.end()) {
                obtainNodes(nxt);
            }
        }
    }
}


vector<vector<Node *>> DFAMinimizer::partition() {
    vector<vector<Node *>> groups = initPartitions();
    int prevCount = -1;
    while (prevCount != groups.size()) {
        vector<vector<Node *>> nextPartition;
        for (auto &group: groups) {
            map<vector<int>, int> partitionID;
            for (auto &node: group) {
                vector<int> goingTo(ASCII);
                for (int i = 1; i < ASCII; i++) {
                    assert(type[node->children[i][0]->id] <= cnt);
                    goingTo[i] = type[node->children[i][0]->id];
                }
                if (partitionID.find(goingTo) == partitionID.end()) {
                    partitionID[goingTo] = nextPartition.size();
                    nextPartition.emplace_back(vector<Node *>());
                }
                nextPartition[partitionID[goingTo]].push_back(node);
            }
        }
        updateType(nextPartition);
        prevCount = groups.size();
        groups = nextPartition;
    }
    return groups;
}

void DFAMinimizer::updateType(vector<vector<Node *>> &v) {
    for (int i = 0; i < v.size(); i++) {
        for (Node *node: v[i]) {
            type[node->id] = i;
        }
    }
}

Node *DFAMinimizer::buildMinimized(vector<vector<Node *>> &groups) {
    vector<Node *> type_to_node(groups.size());
    for (int i = 0; i < groups.size(); i++) {
        type_to_node[i] = new Node(i, false);
        auto& v = type_to_node[i]->acceptingDefS;
        for (auto node: groups[i]) {
            type_to_node[i]->accepting |= node->accepting; // THIS SHOULD USE A SETTER
            // TODO choose the correct accepting string;
            if (node->accepting) {
                if (v.empty())
                    v.push_back(node->acceptingDefS[0]);
                else if (priorities[v[0]] > priorities[node->acceptingDefS[0]]) {
                    v[0] = node->acceptingDefS[0];
                }
            }
        }
    }
    for (int i = 0; i < groups.size(); i++) {
        assert(groups[i].size());
        for (int j = 0; j < ASCII; j++) {
            if (!groups[i][0]->children[j].empty()) {
                type_to_node[i]->children[j].push_back(
                        type_to_node[type[groups[i][0]->children[j][0]->id]]);
            } else {
                type_to_node[i]->children[j].push_back(type_to_node[type[0]]);
            }
        }
    }
    return type_to_node[type[startID]];
}

void DFAMinimizer::freeDFA() {
    for (auto &node: nodes) {
        delete node.second;
    }
}

vector<vector<struct Node *>> DFAMinimizer::initPartitions() {
    type.resize(cnt);
    vector<vector<Node *>> groups(1);
    map<string, int> grp;
    for (auto &node: nodes) {
        auto &v = node.second->acceptingDefS;
        sort(v.begin(), v.end(), [this](const string &a, const string &b) {
            return priorities[a] < priorities[b];
        });
        assert(node.second->id < cnt);
        int node_type = 0;
        if (node.second->accepting) {
            if (grp.find(v[0]) != grp.end()) {
                node_type = grp[v[0]];
            } else {
                node_type = groups.size();
                groups.emplace_back();
                grp[v[0]] = node_type;
            }
        }
        groups[node_type].push_back(node.second);
        type[node.second->id] = node_type;
    }

    return groups;
}
