#include "Lexer.h"
#include "SyntaxTree.h"
#include <string>
#include <memory>
#include <vector>

class Parser {
private:
	
	Lexer lexer;
	inline bool isEqual(std::string target, std::string c) {
		if (target.compare(c) == 0) {
			return true;
		}
		return false;
	}
	std::vector<std::unique_ptr<Expression>> parse_arguments();

public:
	Token token;
	Parser() {}
	void parse();
	
	void prepare(std::string filename);

	
	std::unique_ptr<LiteralExpression> parse_literal();
	
	std::unique_ptr<PranExpression> parse_parn_exp();

	std::unique_ptr<IfStatement> parse_if_statement();
	
	std::unique_ptr<WhileStatement> parse_while_statement();
	
	std::unique_ptr<BinaryExpression> parse_arithmetic_expression();

	std::unique_ptr<BinaryExpression> parse_relation_expression();

	std::unique_ptr<ClassCreatorExpression> parse_creator();

	std::unique_ptr<MethodInvocationExpression> parse_method_invocation();

	std::unique_ptr<MethodDefinition> parse_method();

	std::unique_ptr<Formal> parse_formal();

	std::shared_ptr<ClassNode> parse_class();

	Expression* parse_primary();

	std::unique_ptr<Expression> Parser::parse_expression();

	std::shared_ptr<FeatureTreeNode> parse_feature();

	std::shared_ptr<StatementNode> parse_statements();

	std::shared_ptr<ConditionStatementNode> parse_if_statement();

	std::shared_ptr<ConditionStatementNode> parse_while_statement();
	
	std::shared_ptr<ExpressionTreeNode> parse_pran_expression();

	std::shared_ptr<ExpressionTreeNode> parse_arguments();
	
	std::shared_ptr<DeclarationNode> parse_parameters();

	//std::shared_ptr<ExpressionTreeNode> parse_primary();

	std::shared_ptr<ExpressionTreeNode> parse_method_invocation();

	std::shared_ptr<ExpressionTreeNode> parse_creator();

	std::shared_ptr<ExpressionTreeNode> parse_arithmetic_expression();

	std::shared_ptr<ExpressionTreeNode> parse_relation_expression();

	std::shared_ptr<ExpressionTreeNode> parse_expression();



	//helper
	void print_expression(std::shared_ptr<ExpressionTreeNode> root);
};

