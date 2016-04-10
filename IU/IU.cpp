// IU.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include "Lexer.h"
#include <fstream>
#include "LexerException.h"
#include "Parser.h"

using namespace std;

int main()
{
/*	Lexer iuLexer;
	iuLexer.openFile("c:/code/IU/test/exp_test.IU");
	Token token = iuLexer.getToken();
	cout << token.lexem << endl;
	iuLexer.unget();
	token = iuLexer.getToken();
	cout << token.lexem << endl;
	token = iuLexer.getToken();
	cout << token.lexem << endl;
	/*Token token;
	while (iuLexer.state != EOF_STATE) {
		token = iuLexer.getToken();
		cout << token.type << " " << token.lexem << endl;
	}*/

	Parser parser;
	parser.prepare("c:/code/IU/test/stmt_test.IU");
//	shared_ptr<Expression> exp = parser.parse_expression();
	shared_ptr<Statement> stmt = parser.parse_statement();
	TreePrinter printer;
	printer.visit(stmt);
	
    return 0;
}

