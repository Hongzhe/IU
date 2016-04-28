#pragma once
/* IU is designed to run on Java virtual machine.
 * Therefore, IU assemble is to generate java class file.
 */
#include "ast.h"
#include "SymbolTable.h"
#include "Analyzer.h"
#include <fstream>
#include <iostream>

class Assembler {
private:
	std::ofstream out;
	std::shared_ptr<ClassNode> current_ast;

	void writeInt32(unsigned __int32 val);
	void writeInt16(unsigned __int16 val);
	void genHead();
public:
	Analyzer analyzer;
	void prepare();
	void startGen(std::string dir);
	void end();
	void genConstantPool(BlockSymbolTable*);
	
	//java class is big-endien coded.
	inline static unsigned int littleToBig(unsigned int num) 
	{
		return ((0xFF000000 & num) >> 24) |
			((0x00FF0000 & num) >> 8) |
			((0x0000FF00 & num) << 8) |
			((0x000000FF & num) << 24);
	}

	inline static unsigned __int16 littleToBig(unsigned __int16 num)
	{
		return ((0xFF00 & num) >> 8) |
			((0x00FF & num) << 8);
	}
};