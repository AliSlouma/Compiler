//
// Created by Ali on 6/3/2021.
//
//

#include "ParserData.h"
#include <set>
#include "map"

#ifndef COMPILER_PARSERTABLE_H
#define COMPILER_PARSERTABLE_H
using namespace std;

class ParserTable {

public:
    ParserTable(ParserData* data) {
        this->nonTerminalsMap = data->getProductionMap();
        this->nonTerminal = data->getNonTerminalTokensVector();
        this->start = Token(NON_TERMINAL, data->getStartString());
    }

    map<string, map<Token, ProductionRule>> getFirstSet() {
        return this->firstSet;
    }

    map<Token, set<Token>> GetFollowSet() {
        return this->followSet;
    }

    void createFirstSetTable();

    void CreateFollowSetTable();

private:
    unordered_map<std::string, std::vector<ProductionRule> > nonTerminalsMap;
    vector<Token> nonTerminal;
    map<string, map<Token, ProductionRule>> firstSet;
    Token start;

    map<Token, set<Token>> followSet;

    void FindFollowSet(Token &token);

    void RecursiveFollowAdd(Token &first, Token second);

    bool AddFirstToFollow(Token &token, Token &next_token, set<Token> &follow);

    void recu(ProductionRule rule, const string &nonTerm);
};


#endif //COMPILER_PARSERTABLE_H
