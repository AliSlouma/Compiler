#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "parser_data.h"

class parser {
public:
    parser();

    parser_data *parse(std::string file_name = "input.txt" /* input.txt */);

    std::vector<char> to_postfix(std::vector<char> _vstr);

private:
    static std::vector<std::string> parse_line(std::string str);

    static void recurse_replacing(vstr *_add_vec, const std::string &identifier, imap *definitions);

    static vstr parse_grammar(std::string str);

    static void replace_expression(imap *definitions, imap *expression);
};


#endif //COMPILER_PARSER_H
