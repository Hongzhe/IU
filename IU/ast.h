#pragma once
#include "Lexer.h"
#include <vector>
#include <memory>
#include <string>

//visitor pattern
class IVisitor
{
public:
	virtual void visit(BinaryExpression* node);
	virtual void visit(LiteralExpression* node);
	virtual void visit(MethodInvocationExpression* node);
	virtual void visit(ClassCreatorExpression* node);
	virtual void visit(PranExpression* node);
	virtual void visit(IfStatement* node);
	virtual void visit(WhileStatement* node);
	virtual void visit(MethodDefinition* node);
};

enum ASTNodeType {
	LITERAL_EXP, PRAN_EXP, BINARY_EXP, METHOD_INVOC_EXP, CREATOR_EXP,
	IF_STMT, WHILE_STMT, MEHTOD_DEFINE_STMT,CLASS_NODE, FORMAL
};

class ASTNode {
public:
	unsigned int lineno;
	ASTNodeType node_type;
	virtual void accept(IVisitor* visitor);
};

class Expression : public ASTNode {
public:
	virtual ~Expression() {}
};


class PranExpression : public Expression {
public:
	std::unique_ptr<Expression> exp;
	void accept(IVisitor* visitor) { visitor->visit(this); }
};

class BinaryExpression : public Expression {
public:
	enum type {
		ASSIGN, EQUAL, ADD, MINUS, MUL, DIV, MOD,
		LT, LEQ, GT, GEQ
	};
	Token op;
	std::unique_ptr<Expression> left;
	std::unique_ptr<Expression> right;
	void accept(IVisitor* visitor) { visitor->visit(this); }
};

class LiteralExpression : public Expression {
public:
	LiteralExpression() {}
	Token token;
	void accept(IVisitor* visitor) { visitor->visit(this); }
};

class ClassCreatorExpression : public Expression {
public:
	Token name;
	std::vector<std::unique_ptr<Expression>>arguments;
	void accept(IVisitor* visitor) { visitor->visit(this); }
};

class MethodInvocationExpression : Expression {
public:
	Token name;
	std::vector<std::unique_ptr<Expression>> arguments;
	void accept(IVisitor* visitor) { visitor->visit(this);}

};

class Statement {
public:
	virtual ~Statement() {};
};

class IfStatement : ASTNode 
{
public:
	std::unique_ptr<Expression> condition;
	std::unique_ptr<Statement> block;
	std::unique_ptr<Statement> elsepart;

};

class WhileStatement : ASTNode
{
public:
	std::unique_ptr<Expression> condition;
	std::unique_ptr<Statement> block;
};

class Formal : ASTNode
{
public:
	Token type;
	Token id;
};

class MethodDefinition : ASTNode
{
public:
	Token returntype;
	Token name;
	std::vector<std::unique_ptr<Formal>> arguments;
	std::vector<std::unique_ptr<Statement>> statements;
};

class ClassNode : public ASTNode
{
public:
	Token classname;
	std::vector<std::unique_ptr<MethodDefinition>> methods;
	std::vector<std::unique_ptr<Formal>> fields;
	std::unique_ptr<ClassNode> parent;
	std::string parentname;
};