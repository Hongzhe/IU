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
	iuLexer.openFile("c:/code/IU/test/first.IU");
	Token token;
	while ((token = iuLexer.getToken()).type != TK_EOF) {
		cout << token.type << " " << token.lexem << endl;
	}
	
    return 0;
}

