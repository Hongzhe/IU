#include "Lexer.h"
#include "SyntaxTree.h"
#include <string>
#include <memory>

class Parser {
private:
	void prepare(std::string filename);
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

	std::shared_ptr<ClassTreeNode> parse_class();

	std::shared_ptr<ExpressionTreeNode> parse_primary();

	std::shared_ptr<ExpressionTreeNode> parse_method_invocation();

	std::shared_ptr<ExpressionTreeNode> parse_creator();

	std::shared_ptr<ExpressionTreeNode> parse_term();

	std::shared_ptr<ExpressionTreeNode> parse_arithmetic_expression();

	std::shared_ptr<ExpressionTreeNode> parse_relation_expression();

	std::shared_ptr<ExpressionTreeNode> parse_expression();
};

