cmake_minimum_required(VERSION 3.16)
project(Compiler)
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_CXX_STANDARD 14)

add_executable(Compiler "main.cpp" "Lexical Analyzer/LexicalAnalyzerGenerator.cpp" "Lexical Analyzer/LexicalAnalyzerGenerator.h" "Lexical Analyzer/parser.cpp" "Lexical Analyzer/parser.h" "Lexical Analyzer/parser_data.h" "Lexical Analyzer/Node.cpp" "Lexical Analyzer/Node.h" "Lexical Analyzer/NFA.cpp" "Lexical Analyzer/NFA.h" "Lexical Analyzer/DFAMinimizer.cpp" "Lexical Analyzer/DFAMinimizer.h" "Lexical Analyzer/DFA.cpp" "Lexical Analyzer/DFA.h" "Lexical Analyzer/printTable.cpp" "Token.h" "ProductionRule.h" ParserData.h SyntaxParser.cpp SyntaxParser.h ParserTable.cpp ParserTable.h GrammarFixer.cpp GrammarFixer.h LexicalOutputParser.cpp LexicalOutputParser.h)