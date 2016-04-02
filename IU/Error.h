#pragma once

#include <string>
#include "Lexer.h"

#include <iostream>


void syntax_error(std::string expect, Token token, Lexer& lexer) {
	std::cerr << "Syntax error at line " << lexer.getLineno() << " expect " << expect << " but get a" << token.lexem;
}
