#ifndef COMPILER_PARSERDATA_H
#define COMPILER_PARSERDATA_H

#include "ProductionRule.h"
#include <vector>
#include <string>
#include <unordered_map>

class ParserData {
public:
    ParserData() {}

    ParserData(std::string &str) {
        this->str = str;
    }

    std::string getStartString() const { return str; }

    std::unordered_map<std::string, std::vector<ProductionRule> > &getProductionMap() { return nonTerminalsMap; }

    std::vector<Token> &getNonTerminalTokensVector() { return nonTerminalTokens; }

    std::vector<Token> &getTerminalTokensVector() { return terminalTokens; }

    void setNonTerminalsMap(std::unordered_map<std::string, std::vector<ProductionRule> > &_nonTerminalsMap) {
        this->nonTerminalsMap = _nonTerminalsMap;
        nonTerminalTokens.clear();
        for (const auto &it : nonTerminalsMap)
            nonTerminalTokens.emplace_back(Type::NON_TERMINAL, it.first);
    }

    void setStartingStr(std::string &_str) { this->str = _str; }

private:
    std::unordered_map<std::string, std::vector<ProductionRule> > nonTerminalsMap;
    std::vector<Token> nonTerminalTokens;
    std::vector<Token> terminalTokens;
    std::string str;
};


#endif //COMPILER_PARSERDATA_H
