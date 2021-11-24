#include <fstream>
#include <iostream>
#include <algorithm>
#include <map>
#include "SyntaxParser.h"

#define DEBUG_PARSER false

ParserData *SyntaxParser::parse_syntax_file(std::string file_name) {
    std::ifstream input_file(file_name);
    if (!input_file) {
        std::cerr << "Unable to find input file\nfile_name = " + file_name << std::endl;
        return nullptr;
    }
    std::string str;
    auto *parserData = new ParserData();
    while (std::getline(input_file, str)) {
        parse_line(parserData, str);
    }
    std::map<Token, bool> temp_map;
    for (const auto &it : parserData->getTerminalTokensVector())
        temp_map[it] = true;
    parserData->getTerminalTokensVector().clear();
    for (const auto &it : temp_map)
        parserData->getTerminalTokensVector().push_back(it.first);
    if (DEBUG_PARSER) {
        for (const auto &it : parserData->getProductionMap()) {
            std::cout << it.first << ": ";
            for (auto itt : it.second) {
                for (const auto &ittt : itt.getTokenVector())
                    std::cout << ittt.getTokenString() << " ";
                std::cout << "| ";
            }
            std::cout << std::endl;
        }
        for (const auto &it : parserData->getNonTerminalTokensVector())
            std::cout << it.getTokenString() << std::endl;
        for (const auto &it : parserData->getTerminalTokensVector())
            std::cout << it.getTokenString() << std::endl;
        std::cout << parserData->getStartString() << std::endl;
    }
    return parserData;
}

void SyntaxParser::parse_line(ParserData *parserData, std::string &line) {
    std::size_t idx = line.find('=');
    if (idx == std::string::npos) {
        std::cerr << "Parsing error, Can't find =" << std::endl;
        exit(0);
    }
    idx = 0;
    line += " ";
    std::string non_terminal_token;
    while (idx < line.size() && !isspace(line[idx])) {
        non_terminal_token += line[idx];
        idx++;
    }
    if (parserData->getStartString().empty())
        parserData->setStartingStr(non_terminal_token);
    parserData->getNonTerminalTokensVector().emplace_back(Type::NON_TERMINAL, non_terminal_token);
    while (idx < line.size() && (isspace(line[idx]) || line[idx] == '='))
        idx++;
    std::vector<ProductionRule> production_rules_vec;
    while (idx < line.size()) {
        std::vector<Token> tokens_str_vec = parse_production(parserData, line, idx);
        ProductionRule production_rule;
        for (const auto &it : tokens_str_vec)
            production_rule.addToken(it);
        production_rules_vec.push_back(production_rule);
    }
    parserData->getProductionMap()[non_terminal_token] = production_rules_vec;
}

std::vector<Token> SyntaxParser::parse_production(ParserData *parserData, std::string &line, std::size_t &start_idx) {
    std::vector<Token> tokens_str_vec;
    std::string token_str;
    while (start_idx < line.size() && line[start_idx] != '|') {
        if (!isspace(line[start_idx])) {
            token_str += line[start_idx];
            if (line[start_idx] == '\\') {
                if (start_idx + 1 >= line.size()) {
                    std::cout << "The line \'" + line + "\' malformed line" << std::endl;
                    exit(0);
                }
                token_str += line[++start_idx];
            }
        } else {
            if (token_str.empty()) {
                std::cout << "The line \'" + line + "\' has more than one consecutive spaces." << std::endl;
                start_idx++;
                continue;
            }
            Type token_type = Type::NON_TERMINAL;
            if (token_str[0] == '\'') {
                token_type = Type::TERMINAL;
                token_str = token_str.substr(1, token_str.size() - 2);
                parserData->getTerminalTokensVector().emplace_back(token_type, token_str);
            } else if (token_str == "EPSILON" || token_str == "\\L")
                token_type = Type::EPSILON;
            if (token_str == R"('\L')") {
                std::cerr << R"(Epsilon shouldn't be included inside ' '. Consider using \L or EPSILON)" << std::endl;
            }
            tokens_str_vec.emplace_back(token_type, token_str);
            token_str.clear();
        }
        start_idx++;
    }
    while (start_idx < line.size() && (line[start_idx] == '|' || isspace(line[start_idx]))) start_idx++;
    return tokens_str_vec;
}
