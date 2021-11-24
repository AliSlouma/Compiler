#include "parser.h"
#include <fstream>
#include <regex>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include "iostream"

#define PRINT_PARSING false
const std::string special_char = "&|*+():=";
const std::string ignore_char = "{}[] ";
const std::string stop_char = special_char + ignore_char;
const std::regex range_regex{R"([0-9a-zA-Z]{1}\s*-\s*[0-9a-zA-Z]{1})"};
std::map<char, int> post_fix_priority;

/*
 * + = @
 * * = $
 */
parser::parser() {
    post_fix_priority['|'] = 0;
    post_fix_priority['&'] = 1;
    post_fix_priority['@'] = 3;
    post_fix_priority['$'] = 3;
}

parser_data *parser::parse(std::string file_name) {
    std::ifstream input_file(file_name);
    if (!input_file) {
        std::cerr << "Unable to find input file\nfile_name = " + file_name << std::endl;
        return nullptr;
    }
    std::map<std::string, int> priorities;
    imap regular_expression;
    imap regular_definition;
    vpstr keywords;
    vchar punctuations;
    std::string str;
    int line_counter = 0;
    while (std::getline(input_file, str)) {
        std::vector<std::string> _vstr = parse_line(str);
        if (_vstr[0] == "{") {
            for (int i = 1; i < _vstr.size(); i++) {
                std::string _temp;
                for (int j = 0; j < _vstr[i].size(); j++) {
                    _temp.push_back(_vstr[i][j]);
                    if (j + 1 < _vstr[i].size())
                        _temp.push_back('&');
                }
                priorities[_vstr[i]] = -1;
                keywords.push_back({_vstr[i], _temp});
            }
        } else if (_vstr[0] == "[") {
            for (int i = 1; i < _vstr.size(); i++) {
                punctuations.push_back(_vstr[i].back());
                priorities[_vstr[i]] = -1;
            }
        } else if (_vstr[1] == "=") {
            for (int i = 2; i < _vstr.size(); i++)
                regular_definition[_vstr[0]].push_back(_vstr[i]);
        } else if (_vstr[1] == ":") {
            for (int i = 2; i < _vstr.size(); i++)
                regular_expression[_vstr[0]].push_back(_vstr[i]);
            priorities[_vstr[0]] = line_counter++;
        }
    }
    replace_expression(&regular_definition, &regular_expression);
    cmap _regular_expression;
    for (const auto &it : regular_expression) {
        for (const auto &itt : it.second) {
            if (itt.size() == 1) {
                /* If it's 1 character then it's most probably *,(,),&,| so we'll just take it. */
                if (itt[0] == '+')
                    _regular_expression[it.first].push_back('@');
                else if (itt[0] == '*')
                    _regular_expression[it.first].push_back('$');
                else
                    _regular_expression[it.first].push_back(itt.back());
            } else if (itt.size() == 2 && itt == "\\L") {
                _regular_expression[it.first].push_back('\0');
            } else {
                for (int i = 0; i < itt.size(); i++) {
                    if (itt[i] != '\\') {
                        _regular_expression[it.first].push_back(itt[i]);
                        if (i + 1 < itt.size())
                            _regular_expression[it.first].push_back('&');
                    }
                }
            }
        }
    }
    if (PRINT_PARSING) {
        std::cout << "Regular expressions \n";
        for (auto &it : _regular_expression) {
            std::cout << it.first << " : ";
            for (auto &itt : it.second)
                std::cout << itt;
            std::cout << std::endl;
        }
        std::cout << "Keywords \n";
        for (auto &it : keywords) {
            std::cout << it.first << " ";
        }
        std::cout << "\nPunctuation \n";
        for (auto &it : punctuations) {
            std::cout << it << " ";
        }
        std::cout << "\n";
    }
    return new parser_data(_regular_expression, priorities, keywords, punctuations);
}

std::vector<char> parser::to_postfix(std::vector<char> _vstr) {
    std::stack<char> post_fix_stk;
    std::vector<char> ret;
    for (int i = 0; i < _vstr.size(); i++) {
        if (_vstr[i] == '(') {
            post_fix_stk.push(_vstr[i]);
            continue;
        } else if (_vstr[i] == ')') {
            while (!post_fix_stk.empty() && post_fix_stk.top() != '(') {
                ret.push_back(post_fix_stk.top());
                post_fix_stk.pop();
            }
            post_fix_stk.pop();
        } else if (_vstr[i] == '|' || _vstr[i] == '$' || _vstr[i] == '@' || _vstr[i] == '&') {
            while (!post_fix_stk.empty() && post_fix_priority[_vstr[i]] < post_fix_priority[post_fix_stk.top()]) {
                ret.push_back(post_fix_stk.top());
                post_fix_stk.pop();
            }
            post_fix_stk.push(_vstr[i]);
        } else {
            ret.push_back(_vstr[i]);
        }
    }
    while (!post_fix_stk.empty()) {
        ret.push_back(post_fix_stk.top());
        post_fix_stk.pop();
    }
    return ret;
}

void parser::replace_expression(imap *definitions, imap *expression) {
    for (auto &it : *expression) {
        vstr *def_vec = &(it.second);
        std::vector<std::string> replaced_vec;
        for (const auto &identifier : *def_vec) {
            if (definitions->find(identifier) != definitions->end()) {
                recurse_replacing(&replaced_vec, identifier, definitions);
            } else replaced_vec.push_back(identifier);
        }
        expression->at(it.first) = replaced_vec;
    }
}

void parser::recurse_replacing(vstr *_add_vec, const std::string &identifier, imap *definitions) {
    if (definitions->find(identifier) == definitions->end()) {
        _add_vec->emplace_back(identifier);
        return;
    }
    _add_vec->emplace_back("(");
    for (const auto &it : definitions->at(identifier))
        recurse_replacing(_add_vec, it, definitions);
    _add_vec->emplace_back(")");
}

vstr parser::parse_grammar(std::string str) {
    vstr parsed_line_vec;
    str.pop_back(); // The last character is always a '}' or ']' for correct grammar
    parsed_line_vec.push_back(std::string{str[0]}); // used to identify the type of the grammar
    for (int i = 1; i < str.length();) {
        if (isspace(str[i])) {
            i++;
            continue;
        }
        std::string piece;
        while (i < str.length() && !isspace(str[i]))
            piece += str[i++];
        parsed_line_vec.push_back(piece);
    }
    return parsed_line_vec;
}

vstr parser::parse_line(std::string str) {
    if (!str.empty() && (str[0] == '{' || str[0] == '['))
        return parse_grammar(str);

    vstr parsed_line_vec;
    std::unordered_map<int, std::smatch> regex_match_pos;
    std::smatch smatch;
    for (std::sregex_iterator it = std::sregex_iterator(str.begin(), str.end(), range_regex);
         it != std::sregex_iterator(); it++) {
        smatch = *it;
        regex_match_pos[smatch.position()] = smatch;
    }
    for (int i = 0; i < str.length();) {
        if (ignore_char.find(str[i]) != std::string::npos) {
            i++;
            continue;
        }
        if (special_char.find(str[i]) != std::string::npos) {
            parsed_line_vec.push_back(std::string{str[i]});
            i++;
            continue;
        }
        auto it = regex_match_pos.find(i);
        if (it != regex_match_pos.end()) {
            std::string cur_match = it->second[0];
            i += cur_match.size();
            cur_match = std::regex_replace(cur_match, std::regex{" "}, "");
            // assuming that each range can only be as *-*
            for (char j = cur_match[0]; j <= cur_match[2]; j++) {
                parsed_line_vec.push_back(std::string{j});
                if (j != cur_match[2])
                    parsed_line_vec.push_back("|");
            }
            continue;
        }
        std::string piece;
        while (i < str.length() && stop_char.find(str[i]) == std::string::npos) {
            if (str[i] == '\\') {
                piece += "\\";
                piece += str[++i];
                i++;
            } else piece += str[i++];
        }
        parsed_line_vec.push_back(piece);
    }
    for (int i = 0; i + 1 < parsed_line_vec.size(); i++) {
        if (parsed_line_vec[i] == "&")
            continue;
        if ((parsed_line_vec[i] == "*" || parsed_line_vec[i] == "+" ||
             special_char.find(parsed_line_vec[i]) == std::string::npos)
            && (parsed_line_vec[i + 1] == "(" || special_char.find(parsed_line_vec[i + 1]) == std::string::npos))
            parsed_line_vec.insert(parsed_line_vec.begin() + i + 1, "&");
    }
    return parsed_line_vec;
}