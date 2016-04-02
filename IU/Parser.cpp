#include "stdafx.h"
#include "Lexer.h"
#include "Parser.h"
#include "ParserException.h"
#include "SyntaxTree.h"
#include "Error.h"
#include <memory>
#include <iostream>

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


//basic element of a expression
shared_ptr<ExpressionTreeNode> Parser::parse_primary() {
	shared_ptr<ExpressionTreeNode> node;
	token = lexer.getToken();
	if (token.type == TK_LEFT_PRAN) { //PAR_EXPRESSION
		node = make_shared<ExpressionTreeNode>(EXPRESSION_NODE);
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
	else if (token.type == TK_INT) {
		node = make_shared<ExpressionTreeNode>(LITERAL_NODE);
		node->content = token.lexem;
	}
	else if (isEqual(token.lexem, "true") || isEqual(token.lexem, "false")) {
		node = make_shared<ExpressionTreeNode>(BOOLEAN_NODE);
		node->content = token.lexem;
	}
	else if (token.type == TK_OBJ_ID) {
		node->content = token.lexem;
		token = lexer.getToken();
		if (token.type == TK_LEFT_PRAN) { //method Invocation
			lexer.unget();
			lexer.unget();
			shared_ptr<ExpressionTreeNode> val = parse_method_invocation();
			node->left = val;
		}
		else {
			lexer.unget();
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

//exp [* / % exp]
std::shared_ptr<ExpressionTreeNode> Parser::parse_arithmetic_expression()
{
	shared_ptr<ExpressionTreeNode> node = make_shared<ExpressionTreeNode>(ARITHMETIC_EXP_NODE);
	shared_ptr<ExpressionTreeNode> lval = parse_primary();
	node->left = lval;
	while (true) {
		token = lexer.getToken();
		if (token.type == TK_MULTIPLY || token.type == TK_DIVID || token.type == TK_MOD) {
			node->op = token.lexem;
			auto rval = parse_arithmetic_expression();
			if (rval == nullptr) {
				syntax_error("expression", token, lexer);
			}
			node->right = rval;
		}
		else {
			lexer.unget();
			break;
		}
	}
	
	return node;
}

// exp [+, - = exp]
std::shared_ptr<ExpressionTreeNode> Parser::parse_relation_expression()
{
	shared_ptr<ExpressionTreeNode> node = make_shared<ExpressionTreeNode>(RELATION_EXP_NODE);
	shared_ptr<ExpressionTreeNode> lval = parse_arithmetic_expression();
	node->left = lval;
	while (true) {
		token = lexer.getToken();
		if (token.type == TK_ADD || token.type == TK_MINUS || token.type == TK_EQ) {
			node->op = token.lexem;
			shared_ptr<ExpressionTreeNode> rval = parse_relation_expression();
			if (rval == nullptr) {
				syntax_error("expression", token, lexer); // token
			}
			node->right = rval;
		}
		else {
			lexer.unget();
			break;
		}
	}
	
	return node;
}

shared_ptr<ExpressionTreeNode> Parser::parse_expression()
{
	shared_ptr<ExpressionTreeNode> node = make_shared<ExpressionTreeNode>(EXPRESSION_NODE);
	shared_ptr<ExpressionTreeNode> lval = parse_relation_expression();
	node->left = lval;
	return node;
}


void Parser::print_expression(shared_ptr<ExpressionTreeNode> root)
{
	if (!root) return;
	if (!root->op.empty()) {
		cout << root->op << " ";
	}
	if (root->type == LITERAL_NODE) {
		cout << root->content << " ";
	}
	print_expression(root->left);
	print_expression(root->right);
}