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
	Lexer iuLexer;
	//iuLexer.openFile("c:/code/IU/test/operator.IU");
	Token token;
	//iuLexer.openFile("c:/code/IU/test/exp_test.IU");
	Parser parser;
	parser.prepare("c:/code/IU/test/exp_test.IU");
	shared_ptr<ExpressionTreeNode> root = parser.parse_expression();
	parser.print_expression(root->left);
	/*try {
		while (iuLexer.state != EOF_STATE) {
			token = iuLexer.getToken();
			if(token.type != TK_INIT)
				cout << token.type << " " << token.lexem << endl;
		}
	}
	catch (LexerException e) {
		cout << "catch a exception " << endl;
		cout <<"At line " << e.lineno << " " <<e.what() << endl;
		cout << token.lexem << endl;
	}*/


	
    return 0;
}

