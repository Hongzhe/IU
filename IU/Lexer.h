#pragma once
#include <string>
#include <fstream>
#include <set>

enum Token_Type {
	TK_INIT = 258,
	TK_OBJ_ID = 259,
	TK_TYPE_ID = 260,
	TK_ASSIGN = 261,
	TK_CHAR = 262,
	TK_STR_CONST = 263,
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
	
	std::fstream fin;
	
	Token cur_token;

	char nextChar();
	
	void init_keyword();
public:
	STATE state;

	Lexer();
	
	Token getToken();
	
	void unget();
	
	void unget(Token token);

	void putback(char c);

	void openFile(std::string file);
	
	void closeFile();
	
	std::set<std::string> keywords;
	
	unsigned getLineno() { return lineno; }

	Token getCurToken() { return cur_token; }
	
	void reset() { lineno = 1; }
	
	bool isBinOp(Token token);
};


