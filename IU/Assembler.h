#pragma once
/* IU is designed to run on Java virtual machine.
 * Therefore, IU assemble is to generate java class file.
 */
#include "ast.h"
#include "SymbolTable.h"
#include "Analyzer.h"
#include "ClassFormat.h"
#include "CodeGenVisitor.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>

class Assembler {
private:
	
	std::ofstream out;

	std::shared_ptr<ClassNode> current_ast;
	
	int current_method_index;
	
	BlockSymbolTable* current_method_symbolTable;
	
	BlockSymbolTable* current_scope;

	int current_scope_index;

	std::map<std::string, std::string> field_descriptors;

	std::vector<cp_info*> constant_pool;
	
	//std::map<std::string, int> methodref_index_map;

	std::vector<Field_Method_info*> field_info_v;

	std::vector<Field_Method_info*> method_info_v;
	
	int cur_class_index;

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

	inline void writeInt32(unsigned __int32 val) {
		uint32_t v = littleToBig(val);
		out.write((char*)&v, 4);
	}

	inline void writeInt16(unsigned __int16 val) {
		unsigned __int16 v = littleToBig(val);
		out.write((char*)&v, 2);
	}
	
	inline void writeInt8(uint8_t val) {
		out.write((char*)&val, 1);
	}

	void genHead();
	
	void writeMethodInfo();

	void writeConstantPool(std::vector<cp_info*>&);
	
	void writeFieldOrMethods(std::vector<Field_Method_info*>&);

	void writeCodeAttribute(Code_attribute* codeAttr);

	void extractConstantFromExpStatement(std::shared_ptr<ExpStatement>, std::vector<cp_info*>&);

	void extractConstantFromBlockStatement(std::shared_ptr<BlockStatement>, std::vector<cp_info*>&);

	void extractConstantFromStatement(std::shared_ptr<Statement>, std::vector<cp_info*>&);

	void extractConstantFromMethod(const std::shared_ptr<MethodDefinition>);
	
	void extractConstantFromField(const std::shared_ptr<Formal>, int);
	
	void extractConstantFromExpression(const std::shared_ptr<Expression>, std::vector<cp_info*>&);
	
	void literalExpConstant(const std::shared_ptr<LiteralExpression> node, std::vector<cp_info*>&);
	
	void binaryExpConstant(const std::shared_ptr<BinaryExpression> node, std::vector<cp_info*>&);

	void pranExpConstant(const std::shared_ptr<PranExpression>, std::vector<cp_info*>&);

	void classCreatorConstant(const std::shared_ptr<ClassCreatorExpression> node, std::vector<cp_info*>& pool);
	
	void methodInvocConstant(const std::shared_ptr<MethodInvocationExpression> node, std::vector<cp_info*>& pool);
	
	void declarationConstant(const std::shared_ptr<VariableDeclareExpression> node, std::vector<cp_info*>& pool);
	
	int genMethodRef(std::string, std::string, std::string, std::vector<cp_info*>&);
	
	int genMethodRef(int, int);

	int genClassInfo(std::string, std::vector<cp_info*>&);
	
	int genUTF8Constant(std::string, std::vector<cp_info*>&);
	
	int genNameAndType(std::string, std::string, std::vector<cp_info*>&);
	
	int genNameAndType(int, int, std::vector<cp_info*>&);

	int lookupConstantTable(unsigned __int8 tag, std::string target);
	
	int lookupField(std::string);

	int lookupMethodRef(int class_index, int nameType_index);

	int lookupMethodRef(std::string classname, std::string name, std::string type);

	int Assembler::lookupClassFromConstantPool(std::string target);
	
	int Assembler::lookupUTF8FromConstantPool(std::string target);
	
	int Assembler::lookupNameTypeFromConstantPool(__int16 name, __int16 type);
	
	int Assembler::lookupNameTypeFromConstantPool(std::string name, std::string type);

	bool Assembler::isEqual(int size, unsigned char* bytes, std::string s);

	std::string Assembler::genMethodDescriptor(std::vector<std::shared_ptr<Formal>>& formals, std::string& type);

	std::string Assembler::genFieldDescriptor(std::string& type);

	Code_attribute* Assembler::genCodeAttribute(std::shared_ptr<MethodDefinition> method);

	void freeConstantPool();
public:
	
	friend class CodeGenVisitor;

	CodeGenVisitor codegenVisitor;
	
	Analyzer analyzer;
	
	void prepare();
	
	void startGen(std::string dir);
	
	void end();
	
	void genConstantPool(BlockSymbolTable*);
	
	
};
