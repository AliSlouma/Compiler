//
// Created by omara on 6/3/2021.
//

#include "LexicalOutputParser.h"

void LexicalOutputParser::createParsingTable() {
    map<string, map<Token, ProductionRule>> firstSet = parserTable->getFirstSet();
    map<Token, set<Token>> followSet = parserTable->GetFollowSet();
    string E = "\\L";
    auto epsilon = *(new Token(Type::EPSILON, E));
    for (const auto &nonTerminal : parserData->getNonTerminalTokensVector()) {
        map<Token, ProductionRule> parsingTableRow, nonTerminalfirstSet = firstSet[nonTerminal.getTokenString()];
        for (pair<Token, ProductionRule> rules : nonTerminalfirstSet)
            if (rules.first != epsilon)
                parsingTableRow.insert(rules);
        auto newProd = new ProductionRule();
        if (nonTerminalfirstSet.count(epsilon)) {
            newProd->getTokenVector().push_back(*new Token(Type::EPSILON, ""));
        } else {
            newProd->getTokenVector().push_back(*new Token(Type::SYNC, nonTerminal.getTokenString()));
        }
        for (const auto &rules : followSet[nonTerminal]) {
            parsingTableRow.insert(make_pair(rules, *newProd));
        }
        this->transitionTable.insert(make_pair(nonTerminal, parsingTableRow));
    }
    printTransitionTable();
}

void LexicalOutputParser::parseLexicalOutput(LexicalAnalyzerGenerator *lexGen) {
    cout << endl << ">>parseLexicalOutput<<" << endl;
    vector<Token> tokensStack;
    auto endString = new Token(Type::TERMINAL, "$"), startToken = new Token(Type::NON_TERMINAL,
                                                                            parserData->getStartString());
    tokensStack.push_back(*endString);
    tokensStack.push_back(*startToken);
    string nextLexOutput = lexGen->getNextToken();

    printStack(tokensStack);

    while (!tokensStack.empty() && !nextLexOutput.empty()) {
        auto nextLexToken = *(new Token(Type::TERMINAL, nextLexOutput)), stackTop = tokensStack.back();
        tokensStack.pop_back();
        if (stackTop.getType() == Type::NON_TERMINAL) {
            map<Token, ProductionRule> parsingTableRow = this->transitionTable[stackTop];
            if (parsingTableRow.count(nextLexToken)) {
                vector<Token> tokens = parsingTableRow[nextLexToken].getTokenVector();
                for (int i = tokens.size() - 1; i >= 0; i--)
                    if (tokens[i].getType() != Type::EPSILON)
                        tokensStack.push_back(tokens[i]);
            } else {
                cout << "error: illegal_no_sync(" << stackTop.getTokenString() << "), discard(" << nextLexOutput << ")."
                     << endl;
                tokensStack.push_back(stackTop);
                nextLexOutput = lexGen->getNextToken();
                //nextLexOutput = output[i++];
            }
        } else if (stackTop.getType() == Type::SYNC) {
            cout << "error: extra_with_sync(" << stackTop.getTokenString() << "), discard(" << stackTop.getTokenString()
                 << ")."
                 << endl;
        } else if (stackTop.getType() == Type::TERMINAL) {
            if (stackTop != nextLexToken) {
                cout << "error: missing_input(" << stackTop.getTokenString() << "), inserted_in_input." << endl;
            } else {
                nextLexOutput = lexGen->getNextToken();
            }
        } else continue;
        printStack(tokensStack);
    }
    if (tokensStack.empty() && nextLexOutput.empty()) {
        cout << "victory" << endl;
    } else if (tokensStack.empty() && !nextLexOutput.empty()) {
        cout << "stack became empty but Lexical Analyzer output still contains tokens :";
        while (!nextLexOutput.empty()) {
            cout << nextLexOutput << endl;
            nextLexOutput = lexGen->getNextToken();
        }
    } else {
        cout << "Lexical Analyzer output ended but stack still contains tokens :" << endl;
        printStack(tokensStack);
    }
}

void LexicalOutputParser::printStack(const vector<Token> &stack) {
    for (int i = stack.size() - 1; i > 0; i--) {
        cout << stack[i].getTokenString();
        if (!stack[i].getTokenString().empty())cout << " ";
    }
    cout << endl;
}

string LexicalOutputParser::padStringWithSize(string s, int size) const {
    if (s.length() == size) return s;
    s = " " + s;
    while (s.size() < size) {
        s += ' ';
    }
    return s;
}

void LexicalOutputParser::print_horizontal_line(ofstream &ofstream1, int colCount,const vector<int> &cellSize, int firstcolSize) {
    for (int i = 0; i < colCount; i++) {
        for (int j = 0; j < cellSize[i]; j++) {
            ofstream1 << "=";
        }
        ofstream1 << "++";
    }
    ofstream1 << "\n";
}

void LexicalOutputParser::printTransitionTable() {
    vector<vector<string >> toBePrinted ;
    std::ofstream ofstream1;
    string s;
    ofstream1.open("Transition Table.txt");
    ofstream1 << ">>Transition Table<<" << endl;
    s = "NonTerminals\\Terminals";
    vector<string> temp ;
    temp.push_back(s);
    vector<Token> terminals = this->parserData->getTerminalTokensVector();
    vector<Token> nonTerminals = this->parserData->getNonTerminalTokensVector();
    for (const auto &itr: terminals) {
        s = itr.getTokenString();
        temp.push_back(s);
    }
    toBePrinted.push_back(temp);
    for (const auto &nonTermItr: nonTerminals) {
        vector<string> nonTermTemp ;
        s = nonTermItr.getTokenString();
        nonTermTemp.push_back(s);
        auto transTableRow = transitionTable[nonTermItr];
        for (const auto &termItr: terminals) {
            s = nonTermItr.getTokenString() + "->";
            if (transTableRow.count(termItr)) {
                vector<Token> nonTerminalProd = transTableRow[termItr].getTokenVector();
                if (nonTerminalProd[0].getType() == Type::TERMINAL ||
                    nonTerminalProd[0].getType() == Type::NON_TERMINAL) {
                    for (const auto &prodItr: nonTerminalProd) {
                        s += " " + prodItr.getTokenString();
                    }
                } else if (nonTerminalProd[0].getType() == Type::EPSILON) {
                    s += "\\L";
                } else {
                    s = "SYNC";
                }
            } else {
                s = "";
            }
            nonTermTemp.push_back(s);
        }
        toBePrinted.push_back(nonTermTemp);
    }
    vector<int> sizes(toBePrinted[0].size(),0) ;
    for(auto i =0 ;i<toBePrinted.size();i++){
        for(auto j =0 ;j<toBePrinted[i].size();j++){
            sizes[j] = max(sizes[j],(int)toBePrinted[i][j].size());
        }
    }
    for(auto i =0 ;i<toBePrinted.size();i++){
        for(auto j =0 ;j<toBePrinted[i].size();j++){
            ofstream1<<padStringWithSize(toBePrinted[i][j],sizes[j])<<"||" ;
        }
        ofstream1<<endl ;
        print_horizontal_line(ofstream1,toBePrinted[i].size(),sizes,sizes[0]);
    }    ofstream1.close();
}


