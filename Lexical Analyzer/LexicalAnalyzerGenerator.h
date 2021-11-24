//
// Created by omara on 5/5/2021.
//

#ifndef COMPILER_LEXICALANALYZER_H
#define COMPILER_LEXICALANALYZER_H

#include "Node.h"
#include "parser.h"
#include "NFA.h"
#include "DFA.h"
#include "DFAMinimizer.h"

class LexicalAnalyzerGenerator {
private :
    string rulesFileName ;
    Node *minimized_dfa_node ;
    int i, lastAcc,lastStop;
    Node *acceptingNode ;
    Node *currNode ;
    vector<string> output ;
    vector<string>::iterator outputIterator;
    void printAcceptingDfs();
    void printErr();
public:
    string getNextToken() ;
    void setRulesFileName(string rulesFileName);
    void processProgram(string programFile_name);
    void processRules ();
    LexicalAnalyzerGenerator(string fileName);
};


#endif //COMPILER_LEXICALANALYZER_H
