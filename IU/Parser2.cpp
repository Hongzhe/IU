#include <memory>
#include <vector>
#include <string>
#include "ast.h"
#include "Lexer.h"
#include "Parser.h"
#include "Error.h"


using namespace std;

void Parser::prepare(std::string filename) {
	lexer.openFile(filename);
}

void Parser::parse()
{
	parse_class();
}


shared_ptr<ClassNode> Parser::parse_class() {
	auto node = make_shared<ClassNode>();
	token = lexer.getToken();
	if (token.lexem != "class") {
		syntax_error("class", token, lexer);
		return nullptr;
	}
	token = lexer.getToken();
	if (token.type != TK_TYPE_ID) {
		syntax_error("TYPE ID", token, lexer);
		return nullptr;
	}
	node->classname = token;
	node->lineno = lexer.getLineno();
	token = lexer.getToken();
	if (token.lexem == "inherits") {
		token = lexer.getToken();
		if (token.type != TK_TYPE_ID) {
			syntax_error("TYPE_ID", token, lexer);
			return nullptr;
		}
		node->parentname = token.lexem;
	}
	else { lexer.unget(); }
	token = lexer.getToken();
	if (token.type != TK_LEFT_BRAC) {
		syntax_error("{", token, lexer);
		return nullptr;
	}
	vector<unique_ptr<MethodDefinition>> methods;
	vector<unique_ptr<Formal>> fields;
	while (true) {
		//parse field
		//parse methods
	}

	token = lexer.getToken();
	if (token.type != TK_RIGHT_BRAC) {
		syntax_error("}", token, lexer);
		return nullptr;
	}
	return node;
}

unique_ptr<Formal> Parser::parse_formal()
{
	auto node = make_unique<Formal>();
	Token token = lexer.getToken();
	if (token.type != TK_TYPE_ID) {
		return nullptr;
	}
	node->type = token;
	token = lexer.getToken();
	if (token.type != TK_OBJ_ID) {
		return nullptr;
	}
	node->id = token;
}

unique_ptr<MethodDefinition> Parser::parse_method()
{
	auto node = make_unique<MethodDefinition>();

	return node;
}

unique_ptr<Expression> Parser::parse_expression()
{

}

std::unique_ptr<BinaryExpression> parse_relation_expression()
{
	auto node = make_unique<BinaryExpression>();
	node->node_type = BINARY_EXP;

	return node;

}

std::unique_ptr<BinaryExpression> parse_arithmetic_expression()
{
	auto node = make_unique<BinaryExpression>();
	node->node_type = BINARY_EXP;

	return node;
}


std::unique_ptr<PranExpression> Parser::parse_parn_exp()
{
	auto node = make_unique<PranExpression>();
	node->node_type = PRAN_EXP;
	Token token = lexer.getToken();
	node->exp  = parse_expression();
	token = lexer.getToken();
	if (token.type != TK_RIGHT_PRAN) {
		syntax_error(")", token, lexer);
		node = nullptr;
	}
	return node;
}

vector<unique_ptr<Expression>> Parser::parse_arguments()
{
	vector<unique_ptr<Expression>> arguments;
//	Token token = lexer.getToken();
	unique_ptr<Expression> exp = parse_expression();
	if (!exp) {
		return arguments;
	}
	arguments.push_back(exp);
	while (true) {
		token = lexer.getToken();
		if (token.type == TK_COMMA) {
			lexer.unget();
			break;
		}
		auto argument = parse_expression();
		if (!argument) {
			syntax_error("expression", token, lexer);
			break;
		}
		arguments.push_back(argument);
	}
	return arguments;
}
unique_ptr<ClassCreatorExpression> Parser::parse_creator()
{
	Token token = lexer.getToken();
	token = lexer.getToken();
	if (token.type != TK_TYPE_ID) {
		lexer.unget();
		syntax_error("TYPE_ID", token, lexer);
		return nullptr;
	}
	unique_ptr<ClassCreatorExpression> node = make_unique<ClassCreatorExpression>();
	node->node_type = CREATOR_EXP;
	node->name = token;
	token = lexer.getToken();
	if (token.type != TK_LEFT_PRAN) {
		syntax_error("(", token, lexer);
		lexer.unget();
		return nullptr;
	}
	vector<unique_ptr<Expression>> arguments = parse_arguments();
	token = lexer.getToken();
	if (token.type != TK_RIGHT_PRAN) {
		syntax_error(")", token, lexer);
		return nullptr;
	}
	node->arguments = arguments;
	return node;
}

Expression* Parser::parse_primary()
{
	Token token = lexer.getToken();
	if (token.type == TK_LEFT_PRAN) {
		lexer.unget();
		unique_ptr<Expression> node = parse_parn_exp();
		return node.get();
	}
	else if (token.type == TK_STR_CONST ||
		token.type == TK_INT_CONST) {
		LiteralExpression* node = new LiteralExpression();
		node->node_type = LITERAL_EXP;
		node->token = token;
		return node;
	}
	else if (token.lexem == "new") {
		lexer.unget();
		auto node = parse_creator();
		if (node) {
			return node.get();
		}
		return nullptr;
	}
}
