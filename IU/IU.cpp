// IU.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include "Lexer.h"
#include <fstream>
#include "LexerException.h"

using namespace std;

int main()
{
	/*Lexer iuLexer;
	iuLexer.openFile("c:/code/IU/test/operator.IU");
	Token token;*/

	if (1 == 0 || 3 < 4) {
		cout << "false" << endl;
	}
	else {
		cout << "true" << endl;
	}
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

