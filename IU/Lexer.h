#pragma once
#include <string>
#include <fstream>
#include <set>

enum Token_Type {
	TK_INIT = 258,
	TK_OBJ_ID,
	TK_TYPE_ID,
	TK_ASSIGN,
	TK_CHAR,
	TK_STR_CONST,
	TK_INT_CONST,
	TK_ERROR,
	TK_EQ,
	TK_LE,
	TK_GT,
	TK_LEQ,
	TK_GEQ,
	TK_DIVID,
	TK_MULTIPLY,
	TK_COMMENT,
	TK_ADD,
	TK_MINUS,
	TK_MOD,
	TK_RETURN,
	TK_KEYWORD,
	TK_EOF,
	TK_RIGHT_PRAN,
	TK_LEFT_PRAN,
	TK_RIGHT_BRAC,
	TK_LEFT_BRAC,
	TK_DOT,
	TK_NOT,
	TK_COMMA,
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
	DIVID_STATE,
	ONELINE_COMMENT_STATE,
	MULTILINE_COMMENT_STATE,
};


struct Token {
	Token_Type type = TK_INIT;
	std::string lexem;

	void reset() {
		lexem.clear();
	}
};

class Lexer {
private:
	unsigned lineno;
	
	std::ifstream fin;
	
	Token cur_token;

	char nextChar();
	
	void putBack();

	void init_keyword();
public:
	Lexer();
	Token getToken();
	void unget();
	void openFile(std::string file);
	void closeFile();
	std::set<std::string> keywords;
	STATE state;
	unsigned getLineno() { return lineno; }

	Token getCurToken() { return cur_token; }
};


