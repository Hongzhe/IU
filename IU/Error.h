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

	static void semantical_assign_incompatible_type(std::string& ltype, std::string& rtype, unsigned int lineno)
	{
		std::cerr << "At line " << lineno << " Cannot assign object of type " << rtype << " to a object of type " << ltype << std::endl;
	}

	static void semantical_undefined_type(std::string& type)
	{
		std::cerr << "type " << type << " is undefined." << std::endl;
	}
	
	static void semantical_undefined_type(std::string& type, unsigned int lineno)
	{
		std::cerr << "At line " << lineno << " type " << type << " is undefined." << std::endl;
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

	static void semantical_method_arguments_number(std::string&name,  int n, int m, unsigned int lineno)
	{
		std::cerr << "At line " << lineno << " " << name << " requires " << n << "parameters instead of " << m << std::endl;
	}
	
	static void semantical_method_argument_incompatibal(std::string method, std::string& expect, std::string& given, unsigned int lineno)
	{
		std::cerr << "At line " << lineno << " " << method << "expect " << expect << "but given " << given << std::endl;
	}

	static void semantical_condition_exp(std::string& given, unsigned int lineno)
	{
		std::cerr << "At line " << lineno << " condition expression should be a Bool value instead of " << given << std::endl;
	}
};


