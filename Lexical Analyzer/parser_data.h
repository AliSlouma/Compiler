#ifndef COMPILER_PARSER_DATA_H
#define COMPILER_PARSER_DATA_H

#include <string>
#include <vector>
#include <map>

typedef std::map<std::string, std::vector<std::string> > imap;
typedef std::map<std::string, std::vector<char> > cmap;
typedef std::vector<std::string> vstr;
typedef std::vector<char> vchar;
typedef std::vector<std::pair<std::string, std::string> > vpstr;

class parser_data {
public:
    parser_data(cmap _cmap, std::map<std::string, int> _priority, vpstr _keywords, vchar _punctuation) {
        this->regular_identifiers = _cmap;
        this->regular_identifier_priority = _priority;
        this->keywords = _keywords;
        this->punctuation = _punctuation;
    }

    cmap get_regular_identifiers() const { return regular_identifiers; }

    vpstr get_keywords() const { return keywords; }

    vchar get_punctuation() const { return punctuation; }

    std::map<std::string, int> get_priority() const { return regular_identifier_priority; }

private:
    cmap regular_identifiers;
    std::map<std::string, int> regular_identifier_priority;
    vpstr keywords;
    vchar punctuation;

};

#endif //COMPILER_PARSER_DATA_H
