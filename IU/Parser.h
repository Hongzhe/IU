#include "Lexer.h"
#include "ast.h"
#include <string>
#include <memory>
#include <vector>
#include <map>
class Parser {
private:
	
	Lexer lexer;
	std::map<std::string, int> BinopPrecedence;
	inline bool isEqual(std::string target, std::string c) {
		if (target.compare(c) == 0) {
			return true;
		}
		return false;
	}
	
	std::shared_ptr<ClassNode> Parser::parse_class();

	std::vector<std::shared_ptr<Expression>> parse_arguments();

	std::shared_ptr<MethodDefinition> parse_method();

	std::shared_ptr<MethodInvocationExpression> parse_method_invocation();

	std::shared_ptr<Expression> parse_binary_experssion(int precedence, std::shared_ptr<Expression> left);
	
	std::shared_ptr<ClassCreatorExpression> parse_creator();

	std::shared_ptr<Expression> parse_primary();

	std::unique_ptr<PranExpression> Parser::parse_parn_exp();

	std::shared_ptr<Formal> parse_formal();

	std::shared_ptr<Statement> Parser::parse_exp_statement();

	std::shared_ptr<BlockStatement> parse_block_statement();

	std::shared_ptr<IfStatement> parse_if_stmt();

	std::shared_ptr<WhileStatement> parse_while_stmt();
	
	std::shared_ptr<ExpStatement> parse_return_stmt();

	std::shared_ptr<Expression> Parser::parse_variable_declar();

	int getOperatorPrecedence();

	
public:
	Token token;
	
	Parser();
	
	std::shared_ptr<ClassNode> parse();
	
	void prepare(std::string filename);
	
	std::shared_ptr<Expression> Parser::parse_expression();

	std::shared_ptr<Statement> parse_statement();
};

