#pragma once

class cp_info
{
public:
	cp_info(unsigned __int8 t) : tag(t) {}
	unsigned __int8 tag;
	virtual ~cp_info(){}
};

class CONSTANT_Class_info: public cp_info
{
public:
	CONSTANT_Class_info(unsigned __int8 t) : cp_info(t) {}
	unsigned __int16 name_index;
};

//struct shared by field, method and interface ref
class Fieldref_info : public cp_info
{
public:
	Fieldref_info(unsigned __int8 t): cp_info(t) {}
	unsigned __int16 class_index;
	unsigned __int16 name_and_type_index;
};

class Methodref_info : public cp_info
{
public:
	Methodref_info(unsigned __int8 t) : cp_info(t) {}
	unsigned __int16 class_index;
	unsigned __int16 name_and_type_index;
};

class CONSTANT_String_info : public cp_info
{
public:
	CONSTANT_String_info(unsigned __int8 t) : cp_info(t) {}
	unsigned __int16 string_index;
};

class CONSTANT_Integer_info : public cp_info
{
public:
	unsigned __int32 bytes;
};

class CONSTANT_NameAndType_info: public cp_info
{
public:
	CONSTANT_NameAndType_info(unsigned __int8 t) : cp_info(t) {}
	unsigned __int16 name_index;
	unsigned __int16 descriptor_index;
};

class CONSTANT_Utf8_info : public cp_info
{
public:
	CONSTANT_Utf8_info(unsigned __int8 t) : cp_info(t) {}
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
	uint16_t attribute_name_index;
	uint32_t attribute_length;
	virtual ~Attribute_info();
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
	Attribute_info* attribute;
};
