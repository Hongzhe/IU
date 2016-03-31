#pragma once


#include <exception>
#include <string>

class ParserException :public std::runtime_error
{
public:
	ParserException(const std::string& s, unsigned line) :
		std::runtime_error(s), lineno(line) {}
	ParserException(const std::string& s, unsigned line, std::string lexem) :
		std::runtime_error(s), lineno(line), lexem(lexem){}

	unsigned lineno;
	std::string lexem;
};