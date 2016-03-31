#include "stdafx.h"
#include "Lexer.h"
#include "Parser.h"
#include "ParserException.h"
#include "SyntaxTree.h"
#include "Error.h"
#include <memory>

using namespace std;

void Parser::prepare(std::string filename) {
	lexer.openFile(filename);
}

void Parser::parse()
{
	parse_class();
}


//class Name { features; }
ClassTreeNode* Parser::parse_class()
{
	shared_ptr<TreeNode> node = make_shared<TreeNode>();
	token = lexer.getToken();
	if (lexer.state == EOF_STATE || token.lexem.compare("class") != 0) {
		syntax_error("class", token, lexer);
	}
	token = lexer.getToken();
	if (lexer.state == EOF_STATE || token.type != TK_TYPE_ID) {
		syntax_error("TYPE_ID", token, lexer);
	}
	node->name = token.lexem;
	token = lexer.getToken();
	if (isID(token.lexem, "inherits")) {
		token = lexer.getToken();
		if (token.type != TK_TYPE_ID) {
			syntax_error("TYPE_ID", token, lexer);
		}
	}

	if (lexer.state == EOF_STATE || token.type != TK_LEFT_BRAC) {
		syntax_error("{", token, lexer);
	}
	parse_features();

	token = lexer.getToken(); 
	if (lexer.state == EOF_STATE || token.type != TK_RIGHT_BRAC) {
		syntax_error("}", token, lexer);
	}
	return node;
}


void Parser::parse_features()
{
	parse_declaration();
	parse_methods();
}

void Parser::parse_declaration()
{

}

void Parser::parse_methods() 
{

}