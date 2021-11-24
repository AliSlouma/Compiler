//
// Created by omara on 5/5/2021.
//

#include "LexicalAnalyzerGenerator.h"
#include "printTable.cpp"

LexicalAnalyzerGenerator::LexicalAnalyzerGenerator(string rulesFileName) {
    this->rulesFileName = rulesFileName;
    processRules();
}

void LexicalAnalyzerGenerator::setRulesFileName(string rulesFileName) {
    this->rulesFileName = rulesFileName;
}


void LexicalAnalyzerGenerator::processRules() {
    parser parser;
    NFA nfa;
    parser_data *data = parser.parse();

    vector<Node *> nodeVec;
    for (const auto &it : data->get_regular_identifiers()) {
        vchar tmp = parser.to_postfix(it.second);
        nodeVec.push_back(nfa.generatePattern(tmp, it.first));
    }
    vpstr _vpstr = data->get_keywords();

    for (auto itt : _vpstr) {
        vchar tmp = parser.to_postfix(std::vector<char>{itt.second.begin(), itt.second.end()});
        nodeVec.push_back(nfa.generatePattern(tmp, itt.first));
    }

    vchar _punctuation = data->get_punctuation();
    for (int i = 0; i < _punctuation.size(); i++) {
        vchar v = std::vector<char>{_punctuation[i]};
        string s(1, _punctuation[i]);
        nodeVec.push_back(nfa.generatePattern(v, s));

    }
    Node *root = nfa.generateGraph(nodeVec);
    DFA *dfa = new DFA();
    std::set<Node *> start_nodes;
    start_nodes.insert(root);
    Node *dfa_node = dfa->NFA_To_Dfa(start_nodes);
    DFAMinimizer *dfaMinimizer = new DFAMinimizer(dfa_node, data);
    minimized_dfa_node = dfaMinimizer->minimizeGraph();
    printTable table = printTable(minimized_dfa_node, data->get_priority());
}

void LexicalAnalyzerGenerator::printAcceptingDfs() {
    i = (lastStop = lastAcc);
    cout << acceptingNode->acceptingDefS[0] << endl;
    cout.flush();
    this->output.push_back(acceptingNode->acceptingDefS[0]);
    acceptingNode = nullptr;
    currNode = minimized_dfa_node;
}

void LexicalAnalyzerGenerator::printErr() {
    cout << "_err_" << endl;
    this->output.push_back("_err_");
    lastStop++;
    i = (lastAcc = lastStop);
    acceptingNode = nullptr;
    currNode = minimized_dfa_node;
}

void LexicalAnalyzerGenerator::processProgram(string file_name) {
    output.resize(0);
    std::ifstream input_file(file_name);
    if (!input_file) {
        std::cerr << "Unable to find input file\nfile_name = " + file_name << std::endl;
        return;
    } else cout << ">>>>> " << file_name << " <<<<<" << endl;
    std::string str;
    while (std::getline(input_file, str)) {
        if (str.empty())continue;
        i = 0, lastAcc = (lastStop = -1);
        currNode = minimized_dfa_node;
        if (currNode->accepting) {
            acceptingNode = currNode;
            lastAcc = 0;
        }
        while (i < str.size()) {
            for (; i < str.size(); i++) {
                char chr = str[i];
                unordered_map<char, vector<Node *>> children = currNode->children;
                if (children.count(chr) && children[chr][0]->id != 0) {
                    currNode = children[chr][0];
                    if (currNode->accepting) {
                        lastAcc = i;
                        acceptingNode = currNode;
                    }
                } else if (str[i] == ' ' || str[i] == '\t') {
                    if (lastAcc - lastStop > 0) {
                        printAcceptingDfs();
                    } else if (lastAcc - lastStop == 0 && lastAcc != i - 1) {
                        printErr();
                    } else {
                        lastStop = (lastAcc = i);
                    }
                } else if (lastAcc - lastStop == 0) {
                    printErr();
                } else {
                    printAcceptingDfs();
                }
            }

            if (lastAcc - lastStop > 0/*acceptingNode == nullptr*/) {
                printAcceptingDfs();
            } else if (lastAcc - lastStop == 0 && lastAcc != i - 1){
                printErr();
            }
            i++;
        }
    }
    output.push_back("$");
    this->outputIterator = output.begin();
}

string LexicalAnalyzerGenerator::getNextToken(){
    if(outputIterator==output.end()){
        return "";
    }else {
        string result = *outputIterator;
        outputIterator++;
        return result;
    }
}

