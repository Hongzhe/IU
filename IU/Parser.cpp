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
std::shared_ptr<ClassTreeNode> Parser::parse_class()
{
	shared_ptr<ClassTreeNode> node = make_shared<ClassTreeNode>();
	token = lexer.getToken();
	if (lexer.state == EOF_STATE || token.lexem.compare("class") != 0) {
		syntax_error("class", token, lexer);
	}
	token = lexer.getToken();
	if (lexer.state == EOF_STATE || token.type != TK_TYPE_ID) {
		syntax_error("TYPE_ID", token, lexer);
	}
	node->content = token.lexem;
	token = lexer.getToken();
	if (isEqual(token.lexem, "inherits")) {
		token = lexer.getToken();
		if (token.type != TK_TYPE_ID) {
			syntax_error("TYPE_ID", token, lexer);
		}
	}

	if (lexer.state == EOF_STATE || token.type != TK_LEFT_BRAC) {
		syntax_error("{", token, lexer);
	}
	//parse_features();

	token = lexer.getToken(); 
	if (lexer.state == EOF_STATE || token.type != TK_RIGHT_BRAC) {
		syntax_error("}", token, lexer);
	}
	return node;
}

void Parser::parse_arithmetic_expression() {
	
}
void Parser::parse_relation_expression()
{

}
void Parser::parse_term() {
	//left val
	parse_primary();
	//if(not primary) {}
}

shared_ptr<ExpressionTreeNode> Parser::parse_primary() {

	shared_ptr<ExpressionTreeNode> node;
	token = lexer.getToken();
	if (token.type == TK_LEFT_PRAN) {
		shared_ptr<ExpressionTreeNode> exp = parse_expression();
		token = lexer.getToken();
		if (token.type != TK_RIGHT_PRAN) {
			syntax_error(")", token, lexer);
			lexer.unget();
			exp = nullptr;
		}
		node->left = exp;
	}
	else if (token.type == TK_STR_CONST) {
		node = make_shared<ExpressionTreeNode>(LITERAL_NODE);
		node->content = token.lexem;
	}
	else if (token.type == TK_INT_CONST) {
		node = make_shared<ExpressionTreeNode>(LITERAL_NODE);
		node->content = token.lexem;
	}
	else if (isEqual(token.lexem, "true") || isEqual(token.lexem, "false")) {
		node = make_shared<ExpressionTreeNode>(BOOLEAN_NODE);
		node->content = token.lexem;
	}
	else if (token.type == TK_OBJ_ID) {
		token = lexer.getToken();
		if (token.type == TK_LEFT_PRAN) { //method Invocation
			lexer.unget();
			lexer.unget();
			shared_ptr<ExpressionTreeNode> val = parse_method_invocation();
			node->left = val;
		}
	}
	else if (token.type == TK_KEYWORD && isEqual(token.lexem, "new")) {
		lexer.unget();
		shared_ptr<ExpressionTreeNode> val = parse_creator();
		node->left = val;
	}
	return node;
}


shared_ptr<ExpressionTreeNode> Parser::parse_method_invocation()
{
	shared_ptr<ExpressionTreeNode> node;
	token = lexer.getToken();
	node->content = token.lexem; //method name
	token = lexer.getToken(); // '('
	shared_ptr<ExpressionTreeNode> paramters = parse_expression();
	token = lexer.getToken();
	if (token.type != TK_RIGHT_BRAC) {
		syntax_error(")", token, lexer);
		node = nullptr;
	}
	else {
		node->left = paramters;
	}
	return node;
}

shared_ptr<ExpressionTreeNode> Parser::parse_creator()
{
	shared_ptr<ExpressionTreeNode> node = make_shared<ExpressionTreeNode>(TYPE_CREATOR_NODE);
	token = lexer.getToken(); //keyword new
	token = lexer.getToken(); //TYPE_ID
	node->content = token.lexem;
	token = lexer.getToken();
	if (token.type == TK_LEFT_PRAN) {
		shared_ptr<ExpressionTreeNode> parameters = parse_expression();
		token = lexer.getToken();
		if (token.type != TK_RIGHT_PRAN) {
			syntax_error(")", token, lexer);
			node = nullptr;
		}
		node->left = parameters;
	}
	else {
		syntax_error("(", token, lexer);
		node = nullptr;
	}
	return node;
}

shared_ptr<ExpressionTreeNode> Parser::parse_expression()
{
	//parse_expression()
	token = lexer.getToken();
	while (isEqual(token.lexem,",")) {
		//parse_expression()
		token = lexer.getToken();
	}
	lexer.unget();
}