#include "Lexer.h"
#include <string>


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
	ClassTreeNode* parse_class();
	void parse_features();
	void parse_declaration();
	void parse_methods();
};

