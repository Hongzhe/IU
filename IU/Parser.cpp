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

Parser::Parser() {
	BinopPrecedence["=="] = 1;
	BinopPrecedence["<"] = 10;
	BinopPrecedence[">"] = 10;
	BinopPrecedence["<="] = 10;
	BinopPrecedence[">="] = 10;
	BinopPrecedence["+"] = 20;
	BinopPrecedence["-"] = 20;
	BinopPrecedence["*"] = 30;
	BinopPrecedence["/"] = 30;
	BinopPrecedence["%"] = 30;
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
	vector<shared_ptr<MethodDefinition>> methods;
	vector<unique_ptr<Formal>> fields;
	while (true) {
		//parse field
		//parse methods
		unique_ptr<Formal>field = parse_formal();
		if (field) {
			fields.push_back(field);
		}
		else {
			shared_ptr<MethodDefinition> m = parse_method();
			if (!m) {
				break;
			}
			methods.push_back(m);
		}
	}
	node->fields = fields;
	node->methods = methods;
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
	token = lexer.getToken();
	if (token.type == TK_ASSIGN) {
		shared_ptr<Expression> exp = parse_expression();
		node->val = exp;
	}
	else {
		lexer.unget();
	}
}

shared_ptr<MethodDefinition> Parser::parse_method()
{
	auto node = make_shared<MethodDefinition>();
	node->node_type = METHOD_DEFINE_STMT;
	Token token = lexer.getToken();
	if (token.type != TK_TYPE_ID) {
		syntax_error("TYPE_ID", token, lexer);
		return nullptr;
	}
	node->returntype = token;
	lexer.getToken();
	if (token.type != TK_OBJ_ID) {
		syntax_error("OBJ_ID", token, lexer);
		return nullptr;
	}
	node->name = token;
	lexer.getToken(); //eat '('
	vector<unique_ptr<Formal>> formals;
	unique_ptr<Formal> formal = parse_formal();
	if (formal)
		formals.push_back(formal);
	while (true) {
		token = lexer.getToken();
		if (token.type != TK_COMMA)
			break;
		unique_ptr<Formal> f = parse_formal();
		if (!f)
			break;
		formals.push_back(f);
	}
	node->arguments = formals;
	shared_ptr<Statement> block = parse_block_statement();
	if (!block)
		return nullptr;
	return node;
}
shared_ptr<Statement> Parser::parse_block_statement() 
{
	auto node = make_shared<BlockStatement>();
	node->node_type = BLOCK_STMT;
	vector<shared_ptr<Statement>> statements;
	token = lexer.getToken();
	if (token.type != TK_LEFT_BRAC) {
		syntax_error("(", token, lexer);
		return nullptr;
	}
	while (true) {
		shared_ptr<Statement> stmt = parse_statement();
		if (stmt) {
			statements.push_back(stmt);
		}
		else {
			return nullptr;
		}
		token = lexer.getToken();
		if (token.type == TK_RIGHT_BRAC) {
			break;
		}
	}
	node->stmts = statements;
	return node;
}

shared_ptr<Statement> Parser::parse_exp_statement()
{
	auto node = make_shared<ExpStatement>(parse_expression());
	return node;
}

shared_ptr<Statement> Parser::parse_statement()
{
	shared_ptr<Statement> node;
	node = parse_exp_statement();
	shared_ptr<ExpStatement> p = dynamic_pointer_cast<ExpStatement>(node);
	if (!p->expression) {
		node = parse_block_statement();
	}
	if (!node) {
		node = parse_if_stmt();
	}
	if (!node) {
		node = parse_while_stmt();
	}

	return node;
}

vector<shared_ptr<Statement>> Parser::parse_statements()
{
	vector<shared_ptr<Statement>> statements;
	
}

shared_ptr<WhileStatement> Parser::parse_while_stmt()
{
	auto node = make_shared<WhileStatement>();
	Token token = lexer.getToken();
	if (token.lexem != "while") {
		return nullptr;
	}
	token = lexer.getToken();
	if (token.type != TK_LEFT_PRAN) {
		syntax_error("(", token, lexer);
		return nullptr;
	}
	shared_ptr<Expression> condition = parse_expression();
	token = lexer.getToken();
	if (token.type != TK_RIGHT_PRAN) {
		syntax_error(")", token, lexer);
		return nullptr;
	}
	shared_ptr<Statement> stmt = parse_block_statement();
	if (!stmt) {
		return nullptr;
	}
	return node;
}

shared_ptr<IfStatement> Parser::parse_if_stmt()
{
	auto node = make_shared<IfStatement>();
	node->node_type = IF_STMT;
	Token token = lexer.getToken();
	if (token.lexem != "if")
		return nullptr;
	token = lexer.getToken();
	if (token.type != TK_LEFT_PRAN) {
		syntax_error("(", token, lexer);
		return nullptr;
	}
		
	shared_ptr<Expression> condition = parse_expression();
	token = lexer.getToken();
	if (token.type != TK_RIGHT_PRAN) {
		syntax_error(")", token, lexer);
		return nullptr;
	}
	token = lexer.getToken();
	//parse statement;
	shared_ptr<Statement> stmt = parse_block_statement();
	if (!stmt) return nullptr;
	node->condition = condition;
	node->block = stmt;
	token = lexer.getToken();
	if (token.lexem == "else") {
		shared_ptr<Statement> elsestmt = parse_block_statement();
		if (!elsestmt) {
			return nullptr;
		}
		node->elsepart = elsestmt;
	}
	return node;
}



int Parser::getOperatorPrecedence() 
{
	Token token = lexer.getCurToken();
	int prec = BinopPrecedence[token.lexem];
	return prec < 0 ? -1 : prec;
}

shared_ptr<Expression> Parser::parse_binary_experssion(int precedence, 
		shared_ptr<Expression> left) 
{
	while (1) {
		Token token = lexer.getToken();
		int tokenprec = getOperatorPrecedence();
		if (tokenprec < precedence) {
			return left;
		}
		Token op = token;
		auto right = parse_primary();
		if (!right)
			return nullptr;
		lexer.getToken();
		int nextprec = getOperatorPrecedence();
		if (tokenprec < nextprec) {
			right = parse_binary_experssion(tokenprec + 1, std::move(right));
			if (!right)
				return nullptr;
		}
		left = make_shared<BinaryExpression>(op, std::move(left), std::move(right));
	}
}
shared_ptr<Expression> Parser::parse_expression()
{
	auto left = parse_primary();
	if (!left) {
		return nullptr;
	}
	return parse_binary_experssion(0, std::move(left));
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

vector<shared_ptr<Expression>> Parser::parse_arguments()
{
	vector<shared_ptr<Expression>> arguments;
	shared_ptr<Expression> exp = parse_expression();
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

shared_ptr<ClassCreatorExpression> Parser::parse_creator()
{
	Token token = lexer.getToken();
	token = lexer.getToken();
	if (token.type != TK_TYPE_ID) {
		lexer.unget();
		syntax_error("TYPE_ID", token, lexer);
		return nullptr;
	}
	shared_ptr<ClassCreatorExpression> node = make_shared<ClassCreatorExpression>();
	node->node_type = CREATOR_EXP;
	node->name = token;
	token = lexer.getToken();
	if (token.type != TK_LEFT_PRAN) {
		syntax_error("(", token, lexer);
		lexer.unget();
		return nullptr;
	}
	vector<shared_ptr<Expression>> arguments = parse_arguments();
	token = lexer.getToken();
	if (token.type != TK_RIGHT_PRAN) {
		syntax_error(")", token, lexer);
		return nullptr;
	}
	node->arguments = arguments;
	return node;
}

shared_ptr<MethodInvocationExpression> Parser::parse_method_invocation()
{
	auto node = make_shared<MethodInvocationExpression>();
	Token token = lexer.getToken();
	node->node_type = METHOD_INVOC_EXP;
	node->name = token;
	token = lexer.getToken();//'('
	vector<shared_ptr<Expression>> arguments = parse_arguments();
	token = lexer.getToken();
	if (token.type != TK_RIGHT_PRAN) {
		syntax_error(")", token, lexer);
		return nullptr;
	}
	node->arguments = arguments;

	return node;
}

shared_ptr<Expression> Parser::parse_primary()
{
	Token token = lexer.getToken();
	if (token.type == TK_LEFT_PRAN) {
		lexer.unget();
		shared_ptr<Expression> node = parse_parn_exp();
		return node;
	}
	else if (token.type == TK_STR_CONST ||
		token.type == TK_INT_CONST || token.lexem == "true" 
		|| token.lexem == "false") {
		shared_ptr<LiteralExpression> node = make_shared<LiteralExpression>();
		node->node_type = LITERAL_EXP;
		node->token = token;
		return node;
	}
	else if (token.lexem == "new") {
		lexer.unget();
		auto node = parse_creator();
		if (node) {
			return node;
		}
		return nullptr;
	}
	else if (token.type == TK_OBJ_ID) {
		token = lexer.getToken();
		if (token.type != TK_LEFT_PRAN) {
			shared_ptr<LiteralExpression> n = make_shared<LiteralExpression>();
			n->token = token;
			return n;
		}
		else {
			lexer.unget();
			auto node = parse_method_invocation();
			if (node) {
				return node;
			} 
			return nullptr;
		}
	}
}
