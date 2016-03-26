#include "stdafx.h"
#include "Token.h"
#include <fstream>
#include <iostream>
using namespace std;

Lexer::Lexer() 
{
	lineno = 0;
	init_keyword();
}

/* reserved keyword */
void Lexer::init_keyword() {
	keywords.insert("class");
	keywords.insert("inherits");
	keywords.insert("if");
	keywords.insert("else");
	keywords.insert("while");
	keywords.insert("new");
	keywords.insert("true");
	keywords.insert("false");
	keywords.insert("void");
}

void Lexer::openFile(string filename)
{
	fin.open(filename);
	if (!fin.is_open()) {
		cerr << "fail to open " << filename << endl;
		exit(-1);
	}
}

void Lexer::closeFile()
{
	if (fin && fin.is_open()) {
		fin.close();
	}
}

char Lexer::nextChar()
{
	if (fin.eof()) {
		return EOF;
	}
	char c = fin.get();
	if (c == '\n') {
		lineno++;
	}
	return c;
}

void Lexer::putBack()
{
	fin.unget();
}

Token Lexer::getToken()
{	
	Token token;
	char c = (char)fin.get();
	
	return token;
}