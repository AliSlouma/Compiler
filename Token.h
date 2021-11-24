#ifndef COMPILER_TOKEN_H
#define COMPILER_TOKEN_H

#include <string>

enum Type {
    TERMINAL = 0,
    NON_TERMINAL = 1,
    EPSILON = 2,
    SYNC = 3
};

class Token {
public:
    Token() {}

    Token(Type _type, std::string str) {
        this->type = _type;
        this->str = str;
    }

    Token(const Token &token) {
        this->type = token.type;
        this->str = token.str;
    }

    Type getType() const { return type; }

    std::string getTokenString() const { return str; }

    inline bool operator==(const Token &rhs) {
        return rhs.type == this->type && rhs.getTokenString() == this->getTokenString();
    }

    inline bool operator!=(const Token &rhs) {
        return rhs.type != this->type || rhs.getTokenString() != this->getTokenString();
    }

    inline bool operator<(const Token &rhs) const {
        return rhs.getTokenString() < this->getTokenString();
    }

private:
    Type type;
    std::string str;
};


#endif //COMPILER_TOKEN_H
