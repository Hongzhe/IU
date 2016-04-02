#include "Lexer.h"
#include "SyntaxTree.h"
#include <string>
#include <memory>

class Parser {
private:
	
	Lexer lexer;
	inline bool isEqual(std::string target, std::string c) {
		if (target.compare(c) == 0) {
			return true;
		}
		return false;
	}
public:
	Token token;
	Parser() {}
	void parse();
	
	void prepare(std::string filename);

	std::shared_ptr<ClassTreeNode> parse_class();
	
	std::shared_ptr<FeatureTreeNode> parse_feature();

	std::shared_ptr<StatementNode> parse_statements();

	std::shared_ptr<ConditionStatementNode> parse_if_statement();

	std::shared_ptr<ConditionStatementNode> parse_while_statement();
	
	std::shared_ptr<ExpressionTreeNode> parse_pran_expression();

	std::shared_ptr<ExpressionTreeNode> parse_arguments();
	
	std::shared_ptr<DeclarationNode> parse_parameters();

	std::shared_ptr<ExpressionTreeNode> parse_primary();

	std::shared_ptr<ExpressionTreeNode> parse_method_invocation();

	std::shared_ptr<ExpressionTreeNode> parse_creator();

	std::shared_ptr<ExpressionTreeNode> parse_arithmetic_expression();

	std::shared_ptr<ExpressionTreeNode> parse_relation_expression();

	std::shared_ptr<ExpressionTreeNode> parse_expression();



	//helper
	void print_expression(std::shared_ptr<ExpressionTreeNode> root);
};

