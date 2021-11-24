#include <iostream>
#include "GrammarFixer.h"


Token createState(std::vector<ProductionRule> &rules, ParserData *data, const std::string name) {
    std::vector<ProductionRule> new_rules;
    Token new_state(Type::NON_TERMINAL, name);
    for (auto &rule: rules) {
        if (rule.tokenVec.size() <= 1) continue;
        ProductionRule new_rule;
        new_rule.tokenVec.insert(new_rule.tokenVec.end(), rule.tokenVec.begin() + 1, rule.tokenVec.end());
        new_rule.addToken(new_state);
        new_rules.push_back(new_rule);
    }
    ProductionRule epsilon;
    epsilon.addToken(Token(EPSILON, "\\L"));
    new_rules.push_back(epsilon);
    data->getProductionMap()[name] = new_rules;
    data->getNonTerminalTokensVector().push_back(new_state);
    return new_state;
}

void GrammarFixer::EliminateLeftImmediate(const Token terminal, std::vector<ProductionRule> &rules, ParserData *data) {
    std::vector<ProductionRule> new_rules;
    std::vector<ProductionRule> to_fix;
    for (auto &rule: rules) {
        auto &tokens = rule.tokenVec;
        if (tokens.empty()) continue;
        if (tokens[0] == terminal) {
            to_fix.push_back(rule);
        } else {
            new_rules.push_back(rule);
        }
    }
    if (!to_fix.empty() && new_rules.empty()) {
        std::cerr << "Infinite loop in left recursions, can't fix";
        exit(0);
    }
    if (to_fix.empty()) return;
    Token new_state = createState(to_fix, data, terminal.getTokenString() + "'");
    for (auto &rule: new_rules) {
        rule.addToken(new_state);
    }
    data->getProductionMap()[terminal.getTokenString()] = new_rules;
}

std::vector<ProductionRule> LeftSubstitute(ProductionRule &rule, std::vector<ProductionRule> &v) {
    std::vector<ProductionRule> result;
    auto tokens = rule.tokenVec;
    tokens.erase(tokens.begin());
    for (auto &production: v) {
        ProductionRule new_rule;
        auto &prod_tokens = production.tokenVec;
        if (prod_tokens.empty()) continue;
        if (prod_tokens[0].getType() == EPSILON) {
            new_rule.tokenVec.insert(new_rule.tokenVec.end(), tokens.begin(), tokens.end());
        } else {
            new_rule.tokenVec.insert(new_rule.tokenVec.end(), prod_tokens.begin(), prod_tokens.end());
            new_rule.tokenVec.insert(new_rule.tokenVec.end(), tokens.begin(), tokens.end());
        }
        if (new_rule.tokenVec.empty()) {
            new_rule.addToken(Token(EPSILON, "\\L"));
        }
        result.push_back(new_rule);
    }
    return result;
}

void GrammarFixer::EliminateLeftRecursion(ParserData *data) {
    std::vector<Token> &terminals = data->getNonTerminalTokensVector();
    for (int i = 0; i < terminals.size(); i++) {
        auto &rules = data->getProductionMap()[terminals[i].getTokenString()];
        for (int j = 0; j < i; j++) {
            std::vector<ProductionRule> new_rules;
            for (auto &production: rules) {
                if (!(production.tokenVec[0] == terminals[j])) {
                    new_rules.push_back(production);
                    continue;
                }
                auto &replacing_rules = data->getProductionMap()[terminals[j].getTokenString()];
                // mass replace
                std::vector<ProductionRule> substituted_rules = LeftSubstitute(production, replacing_rules);
                new_rules.insert(new_rules.end(), substituted_rules.begin(), substituted_rules.end());
            }
            rules = new_rules;
        }
        EliminateLeftImmediate(terminals[i], rules, data);
    }
}

void printProductionRules(std::vector<ProductionRule> rules) {
    std::cout << rules.size() << std::endl;
    for (auto it : rules) {
        for (const auto &itt : it.getTokenVector())
            std::cout << itt.getTokenString() << " ";
        std::cout << "|";
    }
    std::cout << "\n";
}

void GrammarFixer::leftFactor(ParserData *data) {
    std::unordered_map<std::string, std::vector<ProductionRule>> rules_map = data->getProductionMap();
    std::unordered_map<std::string, std::vector<ProductionRule>> new_rules_map;
    int current_iteration = 0;
    while (true) {
        for (auto it : rules_map) {
            std::vector<ProductionRule> &rules_vec = it.second;
            for (int i = 0; i < rules_vec.size(); i++) {
                std::vector<int> common_prefix_idx;
                std::vector<Token> matched_tokens;
                common_prefix_idx.push_back(i);
                matched_tokens.push_back(rules_vec[i].getTokenVector()[0]);
                for (int j = 0; j < rules_vec.size(); j++) {
                    if (i == j)
                        continue;
                    if (rules_vec[i].getTokenVector()[0] == rules_vec[j].getTokenVector()[0])
                        common_prefix_idx.push_back(j);
                }
                if (common_prefix_idx.size() == 1) {
                    // No matching prefix with any other production rule. Let's leave
                    continue;
                }
                int best_prefix_match = 0;
                bool match = true;
                while (match) {
                    for (int j : common_prefix_idx) {
                        if (rules_vec[j].getTokenVector().size() <= best_prefix_match + 1) {
                            // One of our production rules have finished.
                            match = false;
                            break;
                        }
                    }
                    if (!match)
                        break;
                    Token &basicToken = rules_vec[common_prefix_idx[0]].getTokenVector()[best_prefix_match + 1];
                    for (const auto &idx : common_prefix_idx) {
                        if (basicToken != rules_vec[idx].getTokenVector()[best_prefix_match + 1]) {
                            // One of the production rules couldn't keep up the prefix match
                            match = false;
                            break;
                        }
                    }
                    if (match) {
                        best_prefix_match++;
                        matched_tokens.push_back(basicToken);
                    }
                }
                std::vector<ProductionRule> removed_production_rules;
                for (int j = 0; j < common_prefix_idx.size(); j++) {
                    removed_production_rules.push_back(rules_vec[common_prefix_idx[j] - j]);
                    rules_vec.erase(rules_vec.begin() + common_prefix_idx[j] - j);
                }
                ProductionRule temp_rule;
                for (auto &cur_token : matched_tokens)
                    temp_rule.addToken(cur_token);
                std::string token_name = "S" + std::to_string(current_iteration++);
                temp_rule.addToken(Token(Type::NON_TERMINAL, token_name));
                rules_vec.push_back(temp_rule);
                for (auto prod_rule : removed_production_rules) {
                    prod_rule.getTokenVector().erase(prod_rule.getTokenVector().begin(),
                                                     prod_rule.getTokenVector().begin() + best_prefix_match + 1);
                    if (prod_rule.getTokenVector().empty())
                        prod_rule.getTokenVector().emplace_back(Type::EPSILON, "\\L");
                    new_rules_map[token_name].push_back(prod_rule);
                }
                // Repeat again until we can't find any other production that matches atleast one of our prefix
                i--;
            }
            new_rules_map[it.first] = rules_vec;
        }
        if (new_rules_map.size() == rules_map.size())
            break;
        rules_map.clear();
        std::swap(new_rules_map, rules_map);
    }
    data->setNonTerminalsMap(new_rules_map);
}


void GrammarFixer::printGrammar(ParserData *data) {
    for (auto &terminal: data->getProductionMap()) {
        std::cout << terminal.first << " -> ";
        for (int i = 0; i < terminal.second.size(); i++) {
            for (auto &token: terminal.second[i].tokenVec) {
                if (token.getType() != EPSILON)
                    std::cout << token.getTokenString() << " ";
                else
                    std::cout << "\\L" << " ";
            }
            if (i + 1 < terminal.second.size())
                std::cout << "| ";
        }
        std::cout << std::endl;
    }
}