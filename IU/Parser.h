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
	Parser() {}
	Token token;
	void parse();
	void parse_class();
	void parse_features();
	void parse_fields();
	void parse_methods();
};

