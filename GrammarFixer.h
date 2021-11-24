#include "ParserData.h"

#ifndef COMPILER_GRAMMARFIXER_H
#define COMPILER_GRAMMARFIXER_H

#endif //COMPILER_GRAMMARFIXER_H

class GrammarFixer {
private:
    void EliminateLeftImmediate(const Token terminal, std::vector<ProductionRule> &rules, ParserData *data);

public:
    void EliminateLeftRecursion(ParserData *data);

    void leftFactor(ParserData *data);

    void printGrammar(ParserData *data);
};