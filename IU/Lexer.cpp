#include "stdafx.h"
#include "Token.h"
#include <fstream>
#include <iostream>
#include <cctype>
#include "LexerException.h"

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
	keywords.insert("return");
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
bool isWitespace(char c)
{
	if (c == ' ' || c == '\t' || c == '\b' || c =='\r'
		|| c == '\v' || c == '\n')
		return true;
	return false;
}


Token Lexer::getToken()
{	
	Token token;
	STATE state = START_STATE;
	int str_escapelevel = 0;
	while (state != DONE_STATE) 
	{
		if (fin.eof()) {
			state = EOF_STATE;
			token.type = TK_EOF;
			break;
		}
		char c = (char)fin.get();
		switch (state)
		{
		case START_STATE:
			if(isWitespace(c)) {} //skip whitespace
			if (islower(c)) {
				state = OBJID_STATE;
				token.type = TK_OBJ_ID;
				token.lexem = c;
			}
			else if (isupper(c)) {
				state = TYPEID_STATE;
				token.type = TK_TYPE_ID;
				token.lexem.append(1,c);
			}
			else if (isdigit(c)) {
				state = NUM_STATE;
				token.type = TK_INT;
				token.lexem = c;
			}
			else if (c == '\'') {
				state = CHAR_STATE;
				token.type = TK_CHAR;
				token.lexem = c;
			}
			else if (c == '\"') {
				state = STR_STATE;
				token.type = TK_STR_CONST;
				token.lexem = c;
			}
			else if (c == '=') {
				token.type = TK_ASSIGN;
				token.lexem = c;
				state = ASSIGN_STATE;
			}
			else if (c == '>') {
				token.type = TK_GT;
				token.lexem = c;
				state = GT_STATE;
			}
			else if (c == '<') {
				token.type = TK_LE;
				token.lexem = c;
				state = LE_STATE;
			}
			else if (c == '/') {
				token.type = TK_DIVID;
				token.lexem = c;
				state = DIVID_STATE;
			}
			else if (c == '*') {
				token.type = TK_MULTIPLY;
				token.lexem = c;
				state = DONE_STATE;
			}
			else if (c == '+') {
				token.type = TK_ADD;
				token.lexem = c;
				state = DONE_STATE;
			}
			else if (c == '-') {
				token.type = TK_MINUS;
				token.lexem = c;
				state = DONE_STATE;
			}
			else if (c == '(') {
				token.type = TK_LEFT_PRAN;
				token.lexem = c;
				state = DONE_STATE;
			}
			else if (c == ')') {
				token.type = TK_RIGHT_PRAN;
				token.lexem = c;
				state = DONE_STATE;
			}
			else if (c == '{') {
				token.type = TK_LEFT_BRAC;
				token.lexem = c;
				state = DONE_STATE;
			}
			else if (c == '}') {
				token.type = TK_RIGHT_PRAN;
				token.lexem = c;
				state = DONE_STATE;
			}
			else if (c == '.') {
				token.type = TK_DOT;
				token.lexem = c;
				state = DONE_STATE;
			}
			break;
		case OBJID_STATE:
			if (isalpha(c) || isdigit(c) || c == '_') {
				token.lexem.append(1, c);
			}
			else {
				state = DONE_STATE;
				putBack();
			}
			break;
		case TYPEID_STATE:
			if (isalpha(c) || isdigit(c) || c == '_') {
				token.lexem.append(1, c);
			}
			else {
				state = DONE_STATE;
				putBack();
			}
			break;
		case NUM_STATE:
			if (isdigit(c)) {
				token.lexem.append(1, c);
			}
			else {
				putBack();
				state = DONE_STATE;
			}
			break;
		case CHAR_STATE:
			if (token.lexem.length() > 0) {
				if (c != '\'') {
					throw LexerException("char contains more than 1 character", lineno);
				}
				else {
					token.lexem.append(1, c);
					state = DONE_STATE;
				}
			}
			else { //token.lexem = 0;
				if (c == '\'') {
					if (token.lexem.empty()) {
						throw LexerException("Emptry char", lineno);
					}
					token.lexem.append(1, c);
					state = DONE_STATE;
				}
				else if (c == '\\') {
					c = nextChar();
					if (c == EOF) {
						state = EOF_STATE;
						break;
					}
					if (c == 'n' || c == 't' || c == 'f' || c == 'b') {
						throw LexerException("Invalid escaped char", lineno);
					}
					token.lexem.append(1, c);
				}
				else {
					token.lexem.append(1, c);
				}
			}
			break;
		case STR_STATE:
			str_escapelevel = 0;
			if (c == '\"') {
				token.lexem.append(1, c);
				state = DONE_STATE;
			
			}
			else if (c == '\\') { 
				state = STR_ESCAPE_STATE;
			}
			else {
				token.lexem.append(1, c);
			}
			break;
		case STR_ESCAPE_STATE:
			if (c == '\"' && str_escapelevel > 0 && (str_escapelevel+1) % 2 == 0) {
				state = DONE_STATE;
			} 
			else if (c == '\\') {
				if (str_escapelevel == 0 || str_escapelevel % 2 != 0) {
					token.lexem.append(1, c);
				}
				state = STR_ESCAPE_STATE;
			}
			else if (c == '\n' &&  str_escapelevel % 2 == 0) {
				token.lexem.append(1, c);
				state = STR_STATE;
			}
			else if (c == '\b' || c == '\t' || c == '\f' || c == '\n') {
				throw LexerException("Invalid escaped char.", lineno);
			}
			else {
				token.lexem.append(1, c);
				state = STR_STATE;
			}
			str_escapelevel++;
			break;
		case ASSIGN_STATE:
			if (c == '=') {
				token.type = TK_EQ;
				token.lexem.append(1, c);
			}
			else {
				putBack();
			}
			state = DONE_STATE;
			break;
		case GT_STATE:
			if (c == '=') {
				token.type = TK_GEQ;
				token.lexem.append(1, c);
			}
			else {
				putBack();
			}
			state = DONE_STATE;
			break;
		case LE_STATE:
			if (c == '=') {
				token.type = TK_LEQ;
				token.lexem.append(1, c);
			}
			else {
				putBack();
			}
			state = DONE_STATE;
			break;
		case DIVID_STATE:
			if (c == '/') {
				state = ONELINE_COMMENT_STATE;
			}
			else if (c == '*') {
				state = MULTILINE_COMMENT_STATE;
			}
			else {
				putBack();
				state = DONE_STATE;
			}
			break;
		case ONELINE_COMMENT_STATE:
			if (c == '\n') {
				state = DONE_STATE;
			}
			token.lexem.append(1, c);
			break;
		case MULTILINE_COMMENT_STATE:
			if (c == '*') {
				c = nextChar();
				if (c == '/') {
					state = DONE_STATE;
				}
				else {
					putBack();
				}
			}
			token.lexem.append(1, c);
			break;
		}

		if (state == DONE_STATE && token.type == TK_OBJ_ID) {
			set<string>::iterator it = keywords.find(token.lexem);
			if (it != keywords.end()) {
				token.type = TK_KEYWORD;
			}
		}
	}
	
	return token;
}