// IU.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include "Token.h"
#include <fstream>
#include "LexerException.h"

using namespace std;

int main()
{
	Lexer iuLexer;
	iuLexer.openFile("c:/code/IU/test/operator.IU");
	Token token;
	try {
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
	}
	
    return 0;
}

