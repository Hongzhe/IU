// IU.cpp : �������̨Ӧ�ó������ڵ㡣
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
	iuLexer.openFile("c:/Users/Hongzhe/Downloads/assign1/first.IU");
	Token token = iuLexer.getToken();
	token = iuLexer.getToken();
	token = iuLexer.getToken();
	cout << token.type << " " << token.lexem << endl;
    return 0;
}

