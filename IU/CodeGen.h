#pragma once
/* IU is designed to run on Java virtual machine.
 * Therefore, IU assemble is to generate java class file.
 */
#include "ast.h"
#include "SymbolTable.h"
#include "Analyzer.h"
#include "ClassFormat.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <map>

class ConstantVisitor;

class Assembler {
private:
	std::ofstream out;

	std::shared_ptr<ClassNode> current_ast;

	std::map<std::string, std::string> field_descriptors;
		
	void writeInt32(unsigned __int32 val);

	void writeInt16(unsigned __int16 val);
	
	void genHead();
	
	void writeConstantPool(std::vector<cp_info*>&);
	
	void writeFieldOrMethods(std::vector<Field_Method_info*>&);

	void extractConstantFromExpStatement(std::shared_ptr<ExpStatement>, std::vector<cp_info*>&);

	void extractConstantFromBlockStatement(std::shared_ptr<BlockStatement>, std::vector<cp_info*>&);

	void extractConstantFromStatement(std::shared_ptr<Statement>, std::vector<cp_info*>&);

	void extractConstantFromMethod(const std::shared_ptr<MethodDefinition>, std::vector<cp_info*>&);
	
	void extractConstantFromField(const std::shared_ptr<Formal>, int, std::vector<cp_info*>&);
	
	void extractConstantFromExpression(const std::shared_ptr<Expression>, std::vector<cp_info*>&);
	
	void literalExpConstant(const std::shared_ptr<LiteralExpression> node, std::vector<cp_info*>&);
	
	void binaryExpConstant(const std::shared_ptr<BinaryExpression> node, std::vector<cp_info*>&);

	void pranExpConstant(const std::shared_ptr<PranExpression>, std::vector<cp_info*>&);

	void classCreatorConstant(const std::shared_ptr<ClassCreatorExpression> node, std::vector<cp_info*>& pool);
	
	void methodInvocConstant(const std::shared_ptr<MethodInvocationExpression> node, std::vector<cp_info*>& pool);
	
	void declarationConstant(const std::shared_ptr<VariableDeclareExpression> node, std::vector<cp_info*>& pool);

	int genMethodRef(std::string, std::string, std::string, std::vector<cp_info*>&);
	
	int genClassInfo(std::string, std::vector<cp_info*>&);
	
	int genUTF8Constant(std::string, std::vector<cp_info*>&);
	
	int genNameAndType(std::string, std::string, std::vector<cp_info*>&);
	
	int genNameAndType(int, int, std::vector<cp_info*>&);

	int lookupConstantTable(unsigned __int8 tag, std::string target, std::vector<cp_info*>&);
	
	int Assembler::lookupClassFromConstantPool(std::string target, std::vector<cp_info*>& pool);
	
	int Assembler::lookupUTF8FromConstantPool(std::string target, std::vector <cp_info*>&);
	
	int Assembler::lookupNameTypeFromConstantPool(__int16 name, __int16 type, std::vector<cp_info*>&);
	
	bool Assembler::isEqual(int size, unsigned char* bytes, std::string s);

public:
	friend ConstantVisitor;

	Analyzer analyzer;
	
	std::vector<Field_Method_info*> field_info_v;

	std::vector<Field_Method_info*> method_info_v;
	
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

class ConstantVisitor :IVisitor
{
public:
	void visit(std::shared_ptr<Expression> exp) 
	{
		switch (exp->node_type)
		{
		case LITERAL_EXP:
			visit(std::dynamic_pointer_cast<LiteralExpression>(exp));
		}
	}

	void visit(std::shared_ptr<PranExpression> node) 
	{

	}

	void visit(std::shared_ptr<BinaryExpression> node) 
	{

	}

	void visit(std::shared_ptr<LiteralExpression> node) 
	{
		if (node->token.type == TK_STR_CONST) {
			
		}
	}
};