#include "stdafx.h"
#include "CodeGen.h"
#include "ClassFormat.h"
using namespace std;

void Assembler::startGen(string dir)
{
	map<string, BlockSymbolTable*> classmap = analyzer.table->classMap;
	for (auto it = classmap.cbegin(); it != classmap.cend(); it++)
	{
		string path = dir + it->first + ".class";
		current_ast = analyzer.classNodes[it->first];
		out.open(path, ios::binary);
		genHead();
		genConstantPool(it->second);
		out.close();
	}
	
}
void Assembler::writeInt32(unsigned __int32 val)
{
	unsigned __int32 v = littleToBig(val);
	out.write((char*)&v, 4);
}
void Assembler::writeInt16(unsigned __int16 val) 
{
	unsigned __int16 v = littleToBig(val);
	out.write((char*)&v, 2);
}

void Assembler::genHead()
{
	unsigned __int32 magic = 0xCAFEBABE;
	writeInt32(magic); unsigned __int32 magic = 0xCAFEBABE;
	unsigned __int16 version = 0x0000;
	writeInt16(version); // write minor version
	version = 0x0033;
	writeInt16(version);
}
void Assembler::genConstantPool(BlockSymbolTable* symboltable)
{
	
}