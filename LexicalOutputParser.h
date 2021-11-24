//
// Created by omara on 6/3/2021.
//

#ifndef COMPILER_LEXICALOUTPUTPARSER_H
#define COMPILER_LEXICALOUTPUTPARSER_H
#include "ParserData.h"
#include "ParserTable.h"
#include "Lexical Analyzer/LexicalAnalyzerGenerator.h"
#include <set>
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

class LexicalOutputParser {

public:
    LexicalOutputParser(ParserData *parserData ,ParserTable *parserTable){
        this->parserData = parserData;
        this->parserTable = parserTable;
    }
    void createParsingTable () ;
    void parseLexicalOutput(LexicalAnalyzerGenerator *lexGen);
private:
    void printStack(const vector<Token> &stack);
    void printTransitionTable();
    static void print_horizontal_line(ofstream &ofstream1, int colCount,const vector<int> &cellSize, int firstcolSize) ;
    string padStringWithSize(string s,int size) const ;
     ParserData *parserData ;
     ParserTable *parserTable ;
     //<nonTerminal,map<nonTerminal,ProductionRule/sync>>
     map<Token,map<Token,ProductionRule>> transitionTable;
};
#endif //COMPILER_LEXICALOUTPUTPARSER_H