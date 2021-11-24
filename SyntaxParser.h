#ifndef COMPILER_SYNTAXPARSER_H
#define COMPILER_SYNTAXPARSER_H

#include "ParserData.h"
#include <string>

class SyntaxParser {
public:
    SyntaxParser() {}

    ParserData *parse_syntax_file(std::string file_name);

private:
    void parse_line(ParserData *parserData, std::string &line);

    static std::vector<Token> parse_production(ParserData *parserData, std::string &line, std::size_t &start_idx);
};


#endif //COMPILER_SYNTAXPARSER_H
