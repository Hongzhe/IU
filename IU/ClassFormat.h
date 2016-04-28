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