// IU.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include "Lexer.h"
#include <fstream>
#include "LexerException.h"
#include "Parser.h"
#include "SymbolTable.h"

using namespace std;

int main()
{
/*	Lexer iuLexer;
	iuLexer.openFile("c:/code/IU/test/exp_test.IU");
	Token token;
	while (iuLexer.state != EOF_STATE) {
		token = iuLexer.getToken();
		cout << token.type << " " << token.lexem << endl;
	}
*/
	Parser parser;
	parser.prepare("c:/code/IU/test/class_test.IU");
	shared_ptr<ClassNode> stmt = parser.parse();
	SymbolTable* table = new SymbolTable();
	BlockSymbolTable* sb = table->addClass(stmt);
	auto sbmap = sb->table;
	for (auto it = sbmap.cbegin(); it != sbmap.cend(); ++it) {
		cout << it->first << " - " << it->second->id << " : " << it->second->type << endl;
	}
	/*TreePrinter printer;
	printer.visit(stmt);*/
    return 0;
}

