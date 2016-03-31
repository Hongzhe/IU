#include "Lexer.h"
#include "SyntaxTree.h"
#include <string>
#include <memory>

class Parser {
private:
	void prepare(std::string filename);
	Lexer lexer;
	inline bool isID(std::string target, std::string c) {
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
	void parse_features();
	void parse_declaration();
	void parse_methods();
	void parse_expression();
};

