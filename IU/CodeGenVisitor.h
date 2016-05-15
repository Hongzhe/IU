#pragma once

#include <memory>
#include <unordered_map>
#include "ast.h"
#include "Assembler.h"
#include "ClassFormat.h"

class Instructions
{
public:
	std::unordered_map<std::string, uint8_t> instructions;
	void init()
	{
		instructions["iconst_0"] = 0x03;
		instructions["iconst_1"] = 0x04;
		instructions["iconst_2"] = 0x05;
		instructions["iconst_3"] = 0x06;
		instructions["iconst_4"] = 0x07;
		instructions["iconst_5"] = 0x08;

		instructions["aload"] = 0x19;
		instructions["aload_0"] = 0x2a;
		instructions["aload_1"] = 0x2b;
		instructions["aload_2"] = 0x2c;
		instructions["aload_3"] = 0x2d;

		instructions["ldc"] = 0x12;
		instructions["iload"] = 0x15;
		instructions["iload_0"] = 0x1a;
		instructions["iload_1"] = 0x1b;
		instructions["iload_2"] = 0x1c;
		instructions["iload_3"] = 0x1d;

		instructions["istore"] = 0x36;
		instructions["istore_0"] = 0x3b;
		instructions["istore_1"] = 0x3c;
		instructions["istore_2"] = 0x3d;
		instructions["istore_3"] = 0x3e;
		instructions["bipush"] = 0x10;
		instructions["sipush"] = 0x11;

		instructions["pop"] = 0x57;
		instructions["pop2"] = 0x58;
		instructions["dup"] = 0x59;
		instructions["dup2"] = 0x5c;
		instructions["dup_x1"] = 0x5a;
		instructions["dup_x2"] = 0x5b;
		instructions["dup2_x1"] = 0x5d;
		instructions["dup2_x2"] = 0x5e;

		instructions["iadd"] = 0x60;
		instructions["isub"] = 0x64;
		instructions["imul"] = 0x68;
		instructions["idiv"] = 0x6c;
		instructions["irem"] = 0x70;
		instructions["ineg"] = 0x74;
		instructions["ishl"] = 0x78;
		instructions["ishr"] = 0x7a;
		instructions["iand"] = 0x7e;
		instructions["iushr"] = 0x7d;

		instructions["ior"] = 0x80;
		instructions["ixor"] = 0x82;
		instructions["iinc"] = 0x84;

		instructions["ifeq"] = 0x99;
		instructions["iflt"] = 0x9b;
		instructions["ifge"] = 0x9c;
		instructions["ifgt"] = 0x9d;
		instructions["ifle"] = 0x9e;
		instructions["if_icmpeq"] = 0x9f;
		instructions["if_icmpne"] = 0xa0;
		instructions["if_icmplt"] = 0xa1;
		instructions["if_icmpge"] = 0xa2;
		instructions["if_icmpgt"] = 0xa3;
		instructions["if_icmple"] = 0xa4;

		instructions["getstatic"] = 0xb2;
		instructions["putstatic"] = 0xb3;
		instructions["getfield"] = 0xb4;
		instructions["putfield"] = 0xb5;
		instructions["invokevirtual"] = 0xb6;
		instructions["invokespecial"] = 0xb7;
		instructions["invokedynamic"] = 0xba;
		instructions["new"] = 0xbb;

		instructions["goto"] = 0xa7;
		instructions["ret"] = 0xa9;
		instructions["ireturn"] = 0xac;
		instructions["areturn"] = 0xb0;
		instructions["return"] = 0xb1;

		instructions["ifnull"] = 0xc6;
		instructions["ifnonnull"] = 0xc7;
	}
};

class Assembler;

class CodeGenVisitor : IVisitor
{
public:
	Assembler* assembler;

	int max_stack;

	int current_stack;

	int max_variable;

	int byte_length;

	bool isStatic;

	std::string current_returntype;

	Instructions instructions;

	std::vector<Instruction*> codes;

	std::vector<std::string>local_variable;
	
	CodeGenVisitor() {}

	CodeGenVisitor(Assembler* assembler, Instructions ins) : assembler(assembler), instructions(ins) {
		max_stack = 0;
		current_stack = 0;
		max_variable = 0;
		byte_length = 0;
		instructions.init();
	}

	~CodeGenVisitor()
	{
		freeCodes();
	}

	void  reset()
	{
		max_stack = 0;
		current_stack = 0;
		max_variable = 0;
		byte_length = 0;
		local_variable.clear();
		//codes.clear();
		freeCodes();
	}

	void appendInstruction(Instruction* instruction);

	void updateStack(int val);

	void freeCodes();

	void visit(std::shared_ptr<MethodDefinition> node);

	void visit(std::shared_ptr<Statement> node);

	void visit(std::shared_ptr<IfStatement> node);

	void visit(std::shared_ptr<WhileStatement> node);

	void visit(std::shared_ptr<BlockStatement> node);

	void visit(std::shared_ptr<ExpStatement> node);

	void visit(std::shared_ptr<Expression> node);

	void visit(std::shared_ptr<BinaryExpression> node);

	void visit(std::shared_ptr<ClassCreatorExpression> node);

	void visit(std::shared_ptr<MethodInvocationExpression>  node);

	void visit(std::shared_ptr<VariableDeclareExpression> node);

	void visit(std::shared_ptr<LiteralExpression> node);

	void visit(std::shared_ptr<PranExpression> node);

	void visit(std::shared_ptr<ClassNode> node) {}

	void visit(std::shared_ptr<Formal> node) {}
private:
	Instruction* visitConditionExp(std::shared_ptr<Expression> node);

	/* generate byte code for class constructor */
	void genClassConstructor(std::shared_ptr<MethodDefinition> node);

	void shuffleRight(int start, std::vector<Instruction*>& v);

	int findLocalVariable(std::string&);

	Instruction* createLoadInstruction(std::string& type, int index, std::string&);
};