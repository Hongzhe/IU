#include "stdafx.h"
#include "CodeGen.h"
#include "ClassFormat.h"
#include <vector>
using namespace std;

void Assembler::prepare()
{
	field_descriptors["Int"] = "I";
	field_descriptors["Bool"] = "Z";
	field_descriptors["Char"] = "C";
}

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

//extrac constant from expression
void Assembler::extractConstantFromExpression(const std::shared_ptr<Expression>, std::vector<cp_info*>&)
{
	
}

void Assembler::literalExpConstant(const shared_ptr<LiteralExpression> node, vector<cp_info*>& pool)
{
	if (node->token.type == TK_STR_CONST) 
	{
		genUTF8Constant(node->token.lexem, pool);
	}
}

void Assembler::classCreatorConstant(const std::shared_ptr<ClassCreatorExpression> node, std::vector<cp_info*>& pool)
{
	string type = node->name.lexem;

	vector<shared_ptr<Expression>> arguments = node->arguments;
	for (auto it = arguments.cbegin(); it != arguments.cend(); it++) {
		extractConstantFromExpression(*it, pool);
	}
}

void Assembler::extractConstantFromField(const std::shared_ptr<Formal> field,
	vector<cp_info*>& pool)
{
	string type = field->type.lexem;
	if (field_descriptors.find(type) != field_descriptors.end()) {
		type = field_descriptors[type];
	}
	else {
		type = "L" + type;
	}
	int type_index = genUTF8Constant(type, pool);
	int id_index = genUTF8Constant(field->id.lexem, pool);
	genNameAndType(id_index, type_index, pool);
	field->val;
}

void Assembler::genConstantPool(BlockSymbolTable* symboltable)
{
	vector<cp_info*> constant_pool;
	string classname = current_ast->classname.lexem;
	genClassInfo(classname, constant_pool);
	genClassInfo("java/lang/Object", constant_pool);
	vector<cp_info*> &b = constant_pool;
	//class inherits from Object. And call Object init method while creating a class.
	genMethodRef("java/lang/Object", "<init>","()V", b);
	vector<shared_ptr<Formal>> fields = current_ast->fields;
	for (int i = 0; i < fields.size(); i++) {
	//	exractConstantFromField(fields[i], b);
	}
}



int Assembler::genClassInfo(std::string classname, vector<cp_info*>& pool)
{
	int found = lookupConstantTable(0x07, classname, pool);
	if (found != -1) return found;
	CONSTANT_Class_info* info = new CONSTANT_Class_info(0x07);
	int name_index = genUTF8Constant(classname, pool);
	info->name_index = name_index;
	pool.push_back(info);
	return pool.size() - 1;
}

int Assembler::genNameAndType(int nameindex, int typeindex, std::vector<cp_info*>& pool)
{
	int found = lookupNameTypeFromConstantPool(nameindex, typeindex, pool);
	if (found != -1) return found;
	CONSTANT_NameAndType_info* info = new CONSTANT_NameAndType_info(0xb0);
	info->name_index = nameindex;
	info->descriptor_index = typeindex;
	pool.push_back(info);
	return pool.size() - 1;
}

int Assembler::genNameAndType(std::string name, std::string type, std::vector<cp_info*>& pool)
{
	int nameindex = genUTF8Constant(name, pool);
	int typeindex = genUTF8Constant(type, pool);
	return genNameAndType(nameindex, typeindex, pool);	
}


int Assembler::genUTF8Constant(std::string content, vector<cp_info*>& pool) 
{
	int found = lookupUTF8FromConstantPool(content, pool);
	if (found != -1) return found;
	CONSTANT_Utf8_info* info = new CONSTANT_Utf8_info(0xa0);
	info->length = content.length();
	info->byte = new unsigned char(info->length);
	for (int i = 0; i < info->length; i++) {
		info->byte[i] = content[i];
	}
	pool.push_back(info);
	return pool.size() - 1;
}

/*method reference contains class info and method's name and return type*/
int Assembler::genMethodRef(string classname, string id, string type, vector<cp_info*>& pool)
{
	int classindex = genClassInfo(classname, pool);
	int idindex = genUTF8Constant(id, pool);
	int typeindex = genUTF8Constant(type, pool);
	int nameAndType = genNameAndType(idindex, typeindex, pool);
	Methodref_info* method_info = new Methodref_info(0xa0);
	method_info->class_index = classindex;
	method_info->name_and_type_index = nameAndType;
	pool.push_back(method_info);
	return pool.size() - 1;
}

int Assembler::lookupNameTypeFromConstantPool(__int16 name, __int16 type, std::vector<cp_info*>& pool)
{
	int index = -1;
	for (int i = 0; i < pool.size(); i++) {
		cp_info* cur = pool[i];
		if (cur->tag != 0xb0) break;
		CONSTANT_NameAndType_info* info = dynamic_cast<CONSTANT_NameAndType_info*>(cur);
		if (info->name_index != name || info->descriptor_index != type)
			continue;
		index = i;
		break;
	}
	return index;
}

int Assembler::lookupUTF8FromConstantPool(std::string target, std::vector < cp_info*>& pool)
{
	int index = -1;
	for (int i = 0; i < pool.size(); i++)
	{
		bool found = true;
		cp_info* info = pool[i];
		if (info->tag != 0x01) continue;
		CONSTANT_Utf8_info* utf8 = dynamic_cast<CONSTANT_Utf8_info*>(info);
		found = isEqual(utf8->length, utf8->byte, target);
		if (found) return i;
	}
	return index;
}

int Assembler::lookupClassFromConstantPool(std::string target, std::vector<cp_info*>& pool)
{
	for (int i = 0; i < pool.size(); i++)
	{
		cp_info* info = pool[i];
		bool found = true;
		CONSTANT_Class_info* classinfo = dynamic_cast<CONSTANT_Class_info*>(info);
		CONSTANT_Utf8_info* utf8 = dynamic_cast<CONSTANT_Utf8_info*>(pool[classinfo->name_index]);
		found = isEqual(utf8->length, utf8->byte, target);
	}
	return -1;
}

int Assembler::lookupConstantTable(unsigned __int8 tag, std::string target, std::vector<cp_info*>& pool)
{
	int index = -1;
	switch (tag)
	{
	case 0x07:
		index = lookupClassFromConstantPool(target, pool);
	case 0x01:
		index = lookupUTF8FromConstantPool(target, pool);
	}
	return -1;
}

bool Assembler::isEqual(int size, unsigned char* bytes, string s)
{
	if (size != s.length()) return false;
	for (int j = 0; j < size; j++) {
		if (bytes[j] != size) {
			return false;
		}
	}
	return true;
}

void Assembler::writeConstantPool(std::vector<cp_info*>& pool)
{
	for (auto it = pool.cbegin(); it != pool.cend(); it++) {
		cp_info* cur = *it;
		unsigned char tag = cur->tag;
		out.write((char*)& tag, 1);
		switch (tag)
		{
		case 0x07:
			CONSTANT_Class_info* class_info = dynamic_cast<CONSTANT_Class_info*>(cur);
			writeInt16(class_info->name_index);
			break;
		case 0x08:
			CONSTANT_String_info* str_info = dynamic_cast<CONSTANT_String_info*>(cur);
			writeInt16(str_info->string_index);
			break;
		case 0x01:
			CONSTANT_Utf8_info* utf8_info = dynamic_cast<CONSTANT_Utf8_info*>(cur);
			writeInt16(utf8_info->length);
			for (int i = 0; i < utf8_info->length; i++) {
				out.write((char*)&utf8_info->byte[i], 1);
			}
			break;
		case 0xb0:
		{
			CONSTANT_NameAndType_info* info = dynamic_cast<CONSTANT_NameAndType_info*>(cur);
			writeInt16(info->name_index);
			writeInt16(info->descriptor_index);
		}
		break;
		case 0x09: 
		case 0x0a:
		case 0x0b:
		{
			Fieldref_info* info = dynamic_cast<Fieldref_info*>(cur);
			writeInt16(info->class_index);
			writeInt16(info->name_and_type_index);
		}
		break;
		case 0x03:
		case 0x04:
		{
			CONSTANT_Integer_info* info = dynamic_cast<CONSTANT_Integer_info*>(cur);
			writeInt32(info->bytes);
		}
			break;
		case 0x12:
		{
			CONSTANT_InvokeDynamic_info* info = dynamic_cast<CONSTANT_InvokeDynamic_info*>(cur);
			writeInt16(info->bootstrap_method_attr_index);
			writeInt16(info->name_and_type_index);
			break;
		}
		}

	}
}