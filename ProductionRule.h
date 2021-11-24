#ifndef COMPILER_PRODUCTIONRULE_H
#define COMPILER_PRODUCTIONRULE_H

#include <vector>
#include <string>
#include "Token.h"

class ProductionRule {
public:
    std::vector<Token> tokenVec;

    ProductionRule() {};

    ProductionRule(const ProductionRule &productionRule) {
        this->tokenVec = productionRule.tokenVec;
    }
    std::vector<Token> getTokenVec(){
        return this->tokenVec;
    }

    std::vector<Token> &getTokenVector() { return tokenVec; }

    void addToken(const Token &token) { tokenVec.push_back(token); }


    inline bool operator<(const ProductionRule &rhs) {
        return rhs.tokenVec < this->tokenVec;
    }



private:

};


#endif //COMPILER_PRODUCTIONRULE_H
