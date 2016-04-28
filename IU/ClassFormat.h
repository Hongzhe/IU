#pragma once

struct CONSTANT_Class_info 
{
	unsigned __int8 tag;
	unsigned __int16 name_index;
};

//struct shared by field, method and interface ref
struct Dref_info
{
	unsigned __int8 tag;
	unsigned __int16 class_index;
	unsigned __int16 name_and_type_index;
};

struct CONSTANT_String_info
{
	unsigned __int8 tag;
	unsigned __int16 string_index;
};

struct CONSTANT_Integer_info
{
	unsigned __int8 tag;
	unsigned __int32 bytes;
};

struct CONSTANT_NameAndType_info
{
	unsigned __int8 tag;
	unsigned __int16 name_index;
	unsigned __int16 descriptor_index;
};

struct CONSTANT_Utf8_info
{
	unsigned __int8 tag;
	unsigned __int16 length;
	unsigned __int8 byte[];
};