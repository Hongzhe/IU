#pragma once

#include <string>
#include "Lexer.h"
#include "SymbolTable.h"
#include <iostream>

class Error 
{
public:
	static void syntax_error(std::string expect, Token token, Lexer& lexer) {
		std::cerr << "Syntax error at line " << lexer.getLineno() << " expect " << expect << " but get a" << token.lexem;
	}

	static void semantical_undefined_var_error(Symbol* symbol)
	{
		std::cerr << symbol->id << " is undefined." << std::endl;
	}

	static void semantical_duplicate_var_error(Symbol* symbol)
	{
		std::cerr << symbol->id << " has been declared before." << std::endl;
	}

	static void semantical_undefined_var_error(std::string id)
	{
		std::cerr << id << " is undefined." << std::endl;
	}

	static void semantical_duplicate_var_error(std::string id)
	{
		std::cerr << id << " has been declared before." << std::endl;
	}

	static void semantical_method_undefined_error(std::string id)
	{
		std::cerr << id << " is not defined. " << std::endl;
	}
	
	static void semantical_operator_incompitable(std::string op)
	{
		std::cerr << op << " is incompatiable." << std::endl;
	}

	static void semantical_class_undefined(std::string type)
	{
		std::cerr << "class " << type << " is undefined." << std::endl;
	}

	static void semantical_method_arguments_number(std::string&name,  int n, int m)
	{
		std::cerr << name << " requires " << n << "parameters instead of " << m << std::endl;
	}
};


