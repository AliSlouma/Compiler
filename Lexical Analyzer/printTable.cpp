#include "Node.h"

class printTable {
    map<int, Node *> nodes;
    map<string, int> priority;
    int attribute_size = 9;
    int attribute_count = 132;
    char hori = '=';
    char separator = '+';
    char vert = '|';
    ofstream output_file;
    const int ASCII = 127;

    string padString(string s) {
        if (s.length() == attribute_size) return s;
        s = " " + s;
        while (s.size() < attribute_size) {
            s += ' ';
        }
        return s;
    }

    void print_horizontal_line() {
        output_file << separator;
        for (int i = 0; i < attribute_count; i++) {
            for (int j = 0; j < attribute_size; j++) {
                output_file << hori;
            }
            output_file << separator;
        }
        output_file << "\n";
    }

    void printHeader() {
        output_file << vert << padString("Node");
        output_file << vert << padString("End node");
        output_file << vert << padString("Ac string");
        output_file << vert << padString("Priority");
        for (int i = 0; i < ASCII; i++) {
            output_file << vert << padString(to_string(i));
        }
        output_file << vert << "\n";
    }

    void obtainNodes(Node *node) {
        nodes[node->id] = node;
        for (auto &input_vec: node->children) {
            for (auto &nxt: input_vec.second) {
                if (nodes.find(nxt->id) == nodes.end()) {
                    obtainNodes(nxt);
                }
            }
        }
    }

    void printNode(Node *node) {
        output_file << vert;
        output_file << padString(to_string(node->id)) << vert;
        if (node->accepting) {
            output_file << padString("Final") << vert;
            output_file << padString((node->acceptingDefS[0])) << vert;
            output_file << padString(to_string(priority[node->acceptingDefS[0]])) << vert;
        } else {
            output_file << padString("Non-Final") << vert;
            output_file << padString("") << vert;
            output_file << padString("N/A") << vert;
        }
        for (int i = 0; i < ASCII; i++) {
            output_file << padString(to_string(node->children[i][0]->id)) << vert;
        }
        output_file << "\n";
    }

public:
    printTable(Node *start, map<string,int> priority, string file_name = "output_table.txt") {
        obtainNodes(start);
        this->priority = priority;
        // PHI IS INCLUDED!!
        this->output_file.open(file_name);
        output_file << nodes.size() << endl;
        print_horizontal_line();
        printHeader();
        print_horizontal_line();
        for (auto& node: nodes) {
            printNode(node.second);
            print_horizontal_line();
        }
        output_file << "The starting node is: " << start->id << endl;
        /*for (auto &node: nodes) {
            cout << node.second->id << "\t";
            for (auto &nxt: node.second->children) {
                for (Node *nxt_node: nxt.second) {
                    if (nxt_node->id)
                        cout << nxt.first << "\t";
                }
            }
            cout << (node.second->accepting?"Accepting":"Not accepting") << " ";
            if (node.second->accepting) {
                for(const auto& i : node.second->acceptingDefS)
                    cout << i << " ";
            }
            cout << endl;
            cout << node.second->id << "\t";
            for (auto &nxt: node.second->children) {
                for (Node *nxt_node: nxt.second) {
                    if (nxt_node->id)
                        cout << nxt_node->id << "\t";
                }
            }
            cout << endl;
        } */
    }
};