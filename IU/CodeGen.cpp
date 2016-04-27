#include "stdafx.h"
#include "CodeGen.h"

using namespace std;

void Assembler::startGen(string src)
{
	out.open(src, ios::binary);
	genConstantPool();
	out.close();
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

void Assembler::genConstantPool()
{
	unsigned __int32 magic = 0xCAFEBABE;
	writeInt32(magic);
}