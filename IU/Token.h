#pragma once
#include <string>
#include <fstream>
#include <set>

enum Token_Type {
	TK_OBJ_ID = 258,
	TK_TYPE_ID,
	TK_INT,
	TK_ASSIGN,
	TK_STR_CONST,
	TK_INT_CONST,
	TK_ERROR
};

struct Token {
	Token_Type type;
	std::string lexem;
};

class Lexer {
private:
	unsigned lineno;
	std::ifstream fin;
	char nextChar();
	void putBack();
	void init_keyword();
public:
	Lexer();
	Token getToken();
	void openFile(std::string file);
	void closeFile();
	std::set<std::string> keywords;
};



