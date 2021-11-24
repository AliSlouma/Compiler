//
// Created by Ali on 6/3/2021.
//

#include <algorithm>
#include "ParserTable.h"
#include <iterator>

using namespace std;

typedef pair<Token, ProductionRule> pairs;

//void ParserTable::createFollowSetTable() {
//
//}
void ParserTable::recu(ProductionRule rule, const string &nonTerm) {

    vector<Token> tokens = rule.getTokenVector();
    for (int i = 0; i < tokens.size(); i++) {

        if (tokens[i].getType() == TERMINAL) {
            string s = tokens[i].getTokenString();
            map<Token, ProductionRule> mapTemp = firstSet[nonTerm];
            mapTemp[tokens[i]] = rule;
            firstSet[nonTerm] = mapTemp;
            return;
        } else if (tokens[i].getType() == EPSILON) {
            string s = tokens[i].getTokenString();
            map<Token, ProductionRule> mapTemp = firstSet[nonTerm];
            mapTemp[tokens[i]] = rule;
            firstSet[nonTerm] = mapTemp;
            return;
        } else if (tokens[i].getType() == NON_TERMINAL) {
            string s = tokens[i].getTokenString();
            vector<ProductionRule> tmp = nonTerminalsMap[tokens[i].getTokenString()];
            for (int j = 0; j < tmp.size(); j++) {
                recu(tmp[j], tokens[i].getTokenString());

            }


            map<Token, ProductionRule> mapTemp = firstSet[tokens[i].getTokenString()];

            Token t(EPSILON, "\\L");
            if (mapTemp.find(t) != mapTemp.end() && i != tokens.size() - 1) {
                mapTemp.erase(t);
                map<Token, ProductionRule> mapTemp2 = firstSet[nonTerm];
                for (const auto &m : mapTemp)
                    mapTemp2[m.first] = rule;

                firstSet[nonTerm] = mapTemp2;
            } else {
                map<Token, ProductionRule> mapTemp2 = firstSet[nonTerm];
                for (const auto &m : mapTemp)
                    mapTemp2[m.first] = rule;

                firstSet[nonTerm] = mapTemp2;
                break;
            }

        }
    }


}

void ParserTable::createFirstSetTable() {
    int size = nonTerminal.size();
    for (int i = 0; i < size; i++) {
        vector<ProductionRule> tmp = nonTerminalsMap[nonTerminal[i].getTokenString()];
        if (firstSet.find(nonTerminal[i].getTokenString()) == firstSet.end())

            for (int j = 0; j < tmp.size(); j++) {
                recu(tmp[j], nonTerminal[i].getTokenString());


            }


    }
}


void ParserTable::CreateFollowSetTable() {
    vector<int> follow_sizes(nonTerminal.size(), 0);
    vector<int> next(nonTerminal.size(), -1);
    followSet[start].insert(Token(TERMINAL, "$"));
    while (follow_sizes != next) {
        next = follow_sizes;
        for (int i = 0; i < nonTerminal.size(); i++) {
            FindFollowSet(nonTerminal[i]);
            follow_sizes[i] = followSet[nonTerminal[i]].size();
        }
    }
}

void ParserTable::FindFollowSet(Token &token) {
    auto &follow_set = followSet[token];
    for (auto &non_terminal: nonTerminalsMap) {
        for (auto &production: non_terminal.second) {
            auto &vec = production.tokenVec;
            int token_idx = find(vec.begin(), vec.end(), token) - vec.begin();
            for (;token_idx < vec.size(); token_idx++) {
                if (token_idx + 1 == vec.size()) {
                    Token prod_token = Token(NON_TERMINAL, non_terminal.first);
                    RecursiveFollowAdd(token, prod_token);
                } else {
                    if (vec[token_idx + 1].getType() == NON_TERMINAL) {
                        bool flag = AddFirstToFollow(token, vec[token_idx + 1], follow_set);
                        if (!flag) break;
                    } else {
                        follow_set.insert(vec[token_idx + 1]);
                        break;
                    }
                }
            }
        }
    }
}


void ParserTable::RecursiveFollowAdd(Token &first, Token second) {
    if (followSet.find(second) == followSet.end()) FindFollowSet(second);
    auto &st = followSet[second];
    followSet[first].insert(st.begin(), st.end());
}

bool ParserTable::AddFirstToFollow(Token &token, Token &next_token, set<Token> &follow) {
    bool flag = false;
    for (auto &first_token: firstSet[next_token.getTokenString()]) {
        if (first_token.first.getType() != EPSILON) {
            follow.insert(first_token.first);
        } else
            flag = true;
    }
    return flag;
}