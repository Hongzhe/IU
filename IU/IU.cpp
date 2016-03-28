// IU.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include "Token.h"
#include <fstream>

using namespace std;

int main()
{
	Lexer iuLexer;
	iuLexer.openFile("d:/code/IU/test/string.IU");
	Token token;
	try {
		while ((token = iuLexer.getToken()).type != TK_EOF) {
			cout << token.type << " " << token.lexem << endl;
		}
	}
	catch (exception e) {
		cout << "catch a exception " << endl;
		cout << e.what() <<  endl;
	}
	
    return 0;
}

