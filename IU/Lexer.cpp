#include "Token.h"
#include <fstream>
#include <iostream>
using namespace std;

Lexer::Lexer() 
{
	lineno = 0;
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

Token getToken()
{

}