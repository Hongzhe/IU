#include "stdafx.h"
#include "Lexer.h"
#include "Parser.h"
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
shared_ptr<ClassTreeNode> Parser::parse_class()
{
	auto node = make_shared<ClassTreeNode>();
	vector<shared_ptr<FeatureTreeNode>> features;
	while (true) {
		shared_ptr<FeatureTreeNode> feature = parse_feature();
		if (!feature) {
			break;
		}
		features.push_back(feature);
	}
	return node;
}

shared_ptr<FeatureTreeNode> Parser::parse_feature()
{
	auto node = make_shared<FeatureTreeNode>();
	token = lexer.getToken();
	if (token.type != TK_TYPE_ID && token.lexem != "void") {
		if (token.type != TK_OBJ_ID) { //parse class constructor
			syntax_error("object id", token, lexer);
			return nullptr;
		}
		node->content = token.lexem;
		token = lexer.getToken();
		if (token.type != TK_LEFT_PRAN) {
			lexer.unget();
			syntax_error("(", token, lexer);
			return nullptr;
		}
		token = lexer.getToken();
		if (token.type == TK_RIGHT_PRAN) {
			return node; //no parameters
		}
		lexer.unget();
		auto parameters = parse_parameters();
		if (!parameters) return nullptr;
		token = lexer.getToken();
		if (token.type != TK_RIGHT_PRAN) {
			lexer.unget();
			syntax_error(")", token, lexer);
			return nullptr;
		}
		return node;
	}
	else if (token.type == TK_TYPE_ID || token.lexem == "void") {
		token = lexer.getToken();
		if (token.type != TK_OBJ_ID) {
			syntax_error("id", token, lexer);
		}
		node->content = token.lexem;
		token = lexer.getToken();
		if (token.type == TK_LEFT_PRAN) { //method
			token = lexer.getToken();
			if (token.type == TK_RIGHT_PRAN) {
				return node; //no parameters
			}
			lexer.unget();
			auto parameters = parse_parameters();
			if (!parameters) {
				return nullptr;
			}
			node->parameters = parameters;
			token = lexer.getToken();
			if (token.type != TK_RIGHT_PRAN) {
				syntax_error(")", token, lexer);
				node = nullptr;
			}
			node->type = MEMBER_METHOD_NDOE;
			shared_ptr<StatementNode> statements = parse_statements();
			node->statements = statements;
		}
		else if (token.type == TK_EQ) { //assignment
			node->type = FIELD_NODE;
			shared_ptr<ExpressionTreeNode> val = parse_expression();
			auto declarator = make_shared<DeclarationNode>();
			declarator->content = node->content;
			declarator->init = val;
			node->declaration = declarator;
		}
	}
	return node;
}

shared_ptr<DeclarationNode> Parser::parse_parameters()
{
	auto node = make_shared<DeclarationNode>();
	token = lexer.getToken();
	if (token.type != TK_TYPE_ID) {
		lexer.unget();
		syntax_error("TYPE ID", token, lexer);
		return nullptr;
	}
	node->type_id = token.lexem;
	token = lexer.getToken();
	if (token.type != TK_OBJ_ID) {
		lexer.unget();
		syntax_error("OBJECT ID", token, lexer);
		return nullptr;
	}
	node->content = token.lexem;
	while (true) {
		token = lexer.getToken();
		if (token.type != TK_COMMA) {
			lexer.unget();
			break;
		}
		auto n = parse_parameters();
		if (!n) break;
		node->next = n;
	}
	return node;
}

shared_ptr<StatementNode> Parser::parse_statements()
{
	shared_ptr<StatementNode> node = make_shared<StatementNode>();
	while (true) {
		shared_ptr<TreeNode> n;
		n = parse_if_statement();
		if (!n) {
			n = parse_while_statement();
		}
		if (!n) {
			n = parse_expression();
		}
		if (!n) {
			break;
		}
		node->statements.push_back(n);
 	}
	return node;
}


shared_ptr<ConditionStatementNode> Parser::parse_while_statement()
{
	auto node = make_shared<ConditionStatementNode>();
	token = lexer.getToken();
	if (token.lexem == "while") {
		auto condition = parse_pran_expression();
		if(!condition) {
			return nullptr;
		}
		node->condition = condition;
		auto body = parse_statements();
		if (!body) {
			return nullptr;
		}
		node->body = body;
	}
	else {
		lexer.unget();
		node = nullptr;
	}
	return node;
}

shared_ptr<ConditionStatementNode> Parser::parse_if_statement()
{
	auto node = make_shared<ConditionStatementNode>();
	token = lexer.getToken();
	if (token.lexem == "if") {
		auto condition = parse_pran_expression();
		if (!condition) return nullptr;
		node->condition = condition;
		auto body = parse_statements();
		if (!body) {
			return nullptr;
		}
		node->body = body;
		token = lexer.getToken();
		if (token.lexem == "else") {
			auto elsebody = parse_statements();
			node->elsebody = elsebody;
		}
		else {
			lexer.unget();
		}
	}
	else {
		lexer.unget();
		node = nullptr;
	}
	return node;
}

std::shared_ptr<ExpressionTreeNode> Parser::parse_pran_expression() 
{
	shared_ptr<ExpressionTreeNode> node;
	token = lexer.getToken();
	if (token.type != TK_LEFT_PRAN) {
		lexer.unget();
		syntax_error("(", token, lexer);
		node = nullptr;
	}
	node = parse_expression();
	token = lexer.getToken();
	if (token.type != TK_RIGHT_PRAN) {
		lexer.unget();
		syntax_error(")", token, lexer);
		node = nullptr;
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

std::shared_ptr<ExpressionTreeNode> Parser::parse_arguments()
{
	auto node = make_shared<ExpressionTreeNode>(ARGUMENTS_NODE);
	auto first = parse_expression();
	node->left = first;
	auto tmp = node->left;
	if (first) {
		while (true) {
			token = lexer.getToken();
			if (token.type != TK_COMMA) {
				lexer.unget();
				break;
			}
			auto next = parse_expression();
			if (!next) {
				syntax_error("expression", token, lexer);
				break;
			}
			node->left = next;
		}
	}
	return node;
}

shared_ptr<ExpressionTreeNode> Parser::parse_method_invocation()
{
	shared_ptr<ExpressionTreeNode> node = make_shared<ExpressionTreeNode>(METHOD_INVOCATION_NODE);
	token = lexer.getToken();
	node->content = token.lexem; //method name
	token = lexer.getToken(); // '('
	shared_ptr<ExpressionTreeNode> paramters = parse_arguments();
	token = lexer.getToken();
	if (token.type != TK_RIGHT_BRAC) {
		syntax_error(")", token, lexer);
		node = nullptr;
		lexer.unget();
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
		shared_ptr<ExpressionTreeNode> parameters = parse_arguments();
		token = lexer.getToken();
		if (token.type != TK_RIGHT_PRAN) {
			syntax_error(")", token, lexer);
			node = nullptr;
			lexer.unget();
		}
		node->left = parameters;
	}
	else {
		syntax_error("(", token, lexer);
		node = nullptr;
		lexer.unget();
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