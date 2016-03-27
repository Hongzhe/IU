#pragma once
#include <string>
#include <fstream>
#include <set>

enum Token_Type {
	TK_OBJ_ID = 258,
	TK_TYPE_ID,
	TK_INT,
	TK_ASSIGN,
	TK_CHAR,
	TK_STR_CONST,
	TK_INT_CONST,
	TK_ERROR
};

struct Token {
	Token_Type type;
	std::string lexem;

	void reset() {
		lexem.clear();
	}
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

enum STATE {
	START_STATE,
	NUM_STATE,
	OBJID_STATE,
	TYPEID_STATE,
	GT_STATE,
	GEQ_STATE,
	LE_STATE,
	LEQ_STATE,
	EQ_STATE,
	ASSIGN_STATE,
	CHAR_STATE,
	STR_STATE,
	STR_ESCAPE_STATE,
	EOF_STATE,
	DONE_STATE,
};

