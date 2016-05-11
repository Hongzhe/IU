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
#include "Analyzer.h"
#include "Assembler.h"
using namespace std;

int main()
{
	/*Lexer iuLexer;
	iuLexer.openFile("c:/code/IU/test/class_test.IU");
	Token token;
	while (iuLexer.state != EOF_STATE) {
		token = iuLexer.getToken();
		cout << "At line " << iuLexer.getLineno() << " " << token.lexem << endl;
	}
	*/
	
	Parser parser;
	//parser.prepare("c:/code/IU/test/class_inherit_test.IU");
//	shared_ptr<ClassNode> inherit_stmt = parser.parse();
	parser.prepare("c:/code/IU/test/Testclass.IU");
	shared_ptr<ClassNode> stmt = parser.parse();


	Analyzer analzyer;
	//BlockSymbolTable* herotable = analzyer.analyze(inherit_stmt);
	//BlockSymbolTable::printBlockSymbolTable(herotable);
	BlockSymbolTable* classtable = analzyer.analyze(stmt);
	//BlockSymbolTable::printBlockSymbolTable(classtable);


	//TreePrinter printer;
	//printer.visit(stmt);
	Assembler assembler;
	assembler.analyzer = analzyer;
	assembler.startGen("c://code//IU//test//");
	
    return 0;

}

