#pragma once
#include <exception>
#include <string>

class LexerException :public std::runtime_error
{
public:
	LexerException(const std::string& s, unsigned line):
	std::runtime_error(s),lineno(line){}
	unsigned lineno;
};