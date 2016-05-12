#pragma once
#include <vector>

enum CONSTANT_TYPE
{
	CONSTANT_CLASS = 0x07,
	CONSTANT_Fieldref = 0x09,
	CONSTANT_Methodref = 0x0a,
	CONSTANT_String = 0x08,
	CONSTANT_Integer = 0x03,
	CONSTANT_NameAndType = 0x0c,
	CONSTANT_Utf8 = 0x01,
	CONSTANT_HOLDER = 0xcc,
};

class Instruction
{
public:
	Instruction() {}
	Instruction(char op, int len) : opcode(op), length(len) {}
	Instruction(char op, uint32_t operand, int len) : opcode(op), length(len), operand(operand) {}

	char opcode; //one byte long opcode
	uint32_t operand;
	int length;
};

class cp_info
{
public:
	cp_info(unsigned __int8 t) : tag(t) {}
	cp_info() {}
	unsigned __int8 tag;
	virtual ~cp_info(){}
};

class CONSTANT_Class_info: public cp_info
{
public:
	CONSTANT_Class_info() {
		cp_info::tag = CONSTANT_CLASS;
	}
	unsigned __int16 name_index;
};

//struct shared by field, method and interface ref
class Fieldref_info : public cp_info
{
public:
	Fieldref_info() {
		cp_info::tag = CONSTANT_Fieldref;
	}
	unsigned __int16 class_index;
	unsigned __int16 name_and_type_index;
};

class Methodref_info : public cp_info
{
public:
	Methodref_info() {
		cp_info::tag = CONSTANT_Methodref;
	}
	unsigned __int16 class_index;
	unsigned __int16 name_and_type_index;
};

class CONSTANT_String_info : public cp_info
{
public:
	CONSTANT_String_info() {
		cp_info::tag = CONSTANT_String;
	}
	unsigned __int16 string_index;
};

class CONSTANT_Integer_info : public cp_info
{
public:
	CONSTANT_Integer_info() {
		cp_info::tag = CONSTANT_Integer;
	}
	unsigned __int32 bytes;
};

class CONSTANT_NameAndType_info: public cp_info
{
public:
	CONSTANT_NameAndType_info() {
		cp_info::tag = CONSTANT_NameAndType;
	}
	unsigned __int16 name_index;
	unsigned __int16 descriptor_index;
};

class CONSTANT_Utf8_info : public cp_info
{
public:
	CONSTANT_Utf8_info() {
		cp_info::tag = CONSTANT_Utf8;
	}
	unsigned __int16 length;
	unsigned char* byte;
};

class CONSTANT_MethodHandle_info : public cp_info
{
public:
	CONSTANT_MethodHandle_info(unsigned __int8 t) : cp_info(t) {}
	unsigned __int8 reference_kind;
	unsigned __int16 reference_index;
};

class CONSTANT_MethodType_info : public cp_info
{
public:
	CONSTANT_MethodType_info(unsigned __int8 t):cp_info(t){}
	unsigned __int16 descriptor_index;
};

class CONSTANT_InvokeDynamic_info : public cp_info
{
public:
	CONSTANT_InvokeDynamic_info(unsigned __int8 t) : cp_info(t) {}
	unsigned __int16 bootstrap_method_attr_index;
	unsigned __int16 name_and_type_index;
};

class Attribute_info {
public:
	Attribute_info(uint16_t name_index) : attribute_name_index(name_index) {}
	uint16_t attribute_name_index;
	uint32_t attribute_length;
	virtual ~Attribute_info() {};
};

class Code_attribute : public Attribute_info
{
public:
	Code_attribute(uint16_t name_index) : Attribute_info(name_index) {}
	~Code_attribute() {}
	uint16_t max_stack;
	uint16_t max_locals;
	uint32_t code_length;
	uint16_t exception_table_length;
	std::vector<Instruction*> code;
	uint16_t attribute_count;
	Attribute_info* attributes;
};

class SourceFile_attribute : public Attribute_info {
public:
	SourceFile_attribute(uint16_t name) : Attribute_info(name) {}
	uint16_t sourcefile_index;
};

class Field_Method_info {
public:
	Field_Method_info() {}
	Field_Method_info(uint16_t flag):access_flag(flag) {}
	Field_Method_info(uint16_t flag, uint16_t name, uint16_t descriptor) : access_flag(flag), 
		name_index(name), descriptor_index(descriptor){}
	uint16_t access_flag;
	uint16_t name_index;
	uint16_t descriptor_index;
	uint16_t attribute_count = 0;
	std::vector<Attribute_info*> attributes;
};
