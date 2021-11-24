#include <iostream>
#include "Lexical Analyzer/DFA.h"
#include "Lexical Analyzer/parser.h"
#include "Lexical Analyzer/DFAMinimizer.h"
#include "Lexical Analyzer/printTable.cpp"
#include "Lexical Analyzer/NFA.h"
#include "Lexical Analyzer/LexicalAnalyzerGenerator.h"
#include "ParserData.h"
#include "GrammarFixer.h"
#include "SyntaxParser.h"
#include "ParserTable.h"
#include "LexicalOutputParser.h"

//#include "ParserTable.cpp"
Node *makeExample1() {
    map<int, Node *> newNodes;
    for (int i = 1; i <= 4; i++) {
        newNodes[i] = new Node(i);
    }
    //node A
    vector<Node *> t1;
    t1.push_back(newNodes[2]);
    newNodes[1]->children.insert(make_pair((char) '0', t1));
    vector<Node *> t2;
    t2.push_back(newNodes[1]);
    t2.push_back(newNodes[3]);
    newNodes[1]->children.insert(make_pair((char) '1', t2));
    //Node B
    newNodes[2]->accepting = true;
    newNodes[2]->acceptingDefS.emplace_back("hello");
    vector<Node *> t3;
    t3.push_back(newNodes[3]);
    newNodes[2]->children.insert(make_pair((char) '0', t3));
    vector<Node *> t4;
    t4.push_back(newNodes[1]);
    newNodes[2]->children.insert(make_pair((char) '1', t4));
    //Node C
    vector<Node *> t5;
    t5.push_back(newNodes[1]);
    newNodes[3]->children.insert(make_pair((char) '0', t5));
    vector<Node *> t6;
    t6.push_back(newNodes[3]);
    newNodes[3]->children.insert(make_pair((char) '1', t6));

    set<Node *> startNodes;
    startNodes.insert(newNodes[1]);
    startNodes.insert(newNodes[2]);
    DFA *dfa = new DFA();
    return dfa->NFA_To_Dfa(startNodes);
}

Node *makeExample2() {
    map<int, Node *> newNodes;
    for (int i = 1; i <= 4; i++) {
        newNodes[i] = new Node(i);
    }
    //node A
    vector<Node *> t1;
    t1.push_back(newNodes[2]);
    t1.push_back(newNodes[3]);
    newNodes[1]->children.insert(make_pair((char) '0', t1));
    vector<Node *> t2;
    t2.push_back(newNodes[3]);
    t2.push_back(newNodes[4]);
    newNodes[1]->children.insert(make_pair((char) '1', t2));
    //Node B
    vector<Node *> t3;
    newNodes[2]->children.insert(make_pair((char) '0', t3));
    vector<Node *> t4;
    t4.push_back(newNodes[4]);
    newNodes[2]->children.insert(make_pair((char) '1', t4));
    //Node C
    vector<Node *> t5;
    t5.push_back(newNodes[1]);
    t5.push_back(newNodes[2]);
    newNodes[3]->children.insert(make_pair((char) '0', t5));
    vector<Node *> t6;
    t6.push_back(newNodes[3]);
    newNodes[3]->children.insert(make_pair((char) '1', t6));
    //Node D
    newNodes[4]->accepting = true;
    newNodes[4]->acceptingDefS.emplace_back("hello");
    vector<Node *> t7;
    newNodes[4]->children.insert(make_pair((char) '0', t7));
    vector<Node *> t8;
    t8.push_back(newNodes[1]);
    t8.push_back(newNodes[3]);
    newNodes[4]->children.insert(make_pair((char) '1', t8));
    set<Node *> startNodes;
    startNodes.insert(newNodes[1]);
    startNodes.insert(newNodes[2]);
    DFA *dfa = new DFA();
    return dfa->NFA_To_Dfa(startNodes);
}

ParserData *GrammarFixTest() {
    std::string start = "S";
    std::string terminals[] = {"A", "S"};
    std::string nonterminals[] = {"a", "b", "c", "d", "f"};
    ParserData *data = new ParserData(start);
    auto &productions = data->getProductionMap();
    vector<ProductionRule> v1;
    ProductionRule r11;
    r11.tokenVec.emplace_back(NON_TERMINAL, terminals[0]);
    r11.tokenVec.emplace_back(TERMINAL, nonterminals[0]);
    v1.push_back(r11);
    ProductionRule r12;
    r12.tokenVec.emplace_back(TERMINAL, nonterminals[1]);
    v1.push_back(r12);
    productions["S"] = v1;
    v1.clear();
    ProductionRule r21;
    r21.tokenVec.emplace_back(NON_TERMINAL, terminals[0]);
    r21.tokenVec.emplace_back(TERMINAL, nonterminals[2]);
    v1.push_back(r21);
    ProductionRule r22;
    r22.tokenVec.emplace_back(NON_TERMINAL, terminals[1]);
    r22.tokenVec.emplace_back(TERMINAL, nonterminals[3]);
    v1.push_back(r22);
    ProductionRule r23;
    r23.tokenVec.emplace_back(TERMINAL, nonterminals[4]);
    v1.push_back(r23);
    productions["A"] = v1;
    data->getNonTerminalTokensVector().emplace_back(NON_TERMINAL, terminals[0]);
    data->getNonTerminalTokensVector().emplace_back(NON_TERMINAL, terminals[1]);
    return data;
}

int main() {
    LexicalAnalyzerGenerator *lexicalAnalyzerGenerator = new LexicalAnalyzerGenerator("input.txt");
    lexicalAnalyzerGenerator->processProgram("test_program.txt");
    SyntaxParser syntaxParser;
    ParserData *data = syntaxParser.parse_syntax_file("input_syntax.txt");
    GrammarFixer fixer;
    fixer.leftFactor(data);
    fixer.EliminateLeftRecursion(data);
    fixer.printGrammar(data);

    ParserTable *parserTable1 = new ParserTable(data);

    parserTable1->createFirstSetTable();

    cout << "\n";
    for (const auto &x: parserTable1->getFirstSet()) {
        cout << x.first << " ==> ";
        for (const auto &y: x.second) {
            if (y.first.getType() != EPSILON) {
                cout << y.first.getTokenString() << " " << ">>>>>";


            } else
                cout << "EPSILON " << ">>>>>";
            for(const auto& v :y.second.tokenVec)
                cout << v.getTokenString() << " ";
        }
        cout << "\n";

    }

    cout << endl;
    parserTable1->CreateFollowSetTable();
    for (const auto &x: parserTable1->GetFollowSet()) {
        cout << x.first.getTokenString() << " ==> ";
        for (const auto &y: x.second)
            if (y.getType() != EPSILON)
                cout << y.getTokenString() << " ";
            else
                cout << "EPSILON ";


        cout << "\n";

    }

    LexicalOutputParser *lex = new LexicalOutputParser(data,parserTable1);
    lex->createParsingTable();
    lex->parseLexicalOutput(lexicalAnalyzerGenerator);
    return 0;
}