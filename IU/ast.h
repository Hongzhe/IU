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
	IF_STMT, WHILE_STMT, METHOD_DEFINE_STMT,CLASS_NODE, FORMAL, BLOCK_STMT
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
	std::shared_ptr<Expression> exp;
	void accept(IVisitor* visitor) { visitor->visit(this); }
};

class BinaryExpression : public Expression {
public:
	BinaryExpression(Token op, std::shared_ptr<Expression> l,
		std::shared_ptr<Expression> r) : op(op), left(l), right(r) {}
	BinaryExpression() {}
	Token op;
	std::shared_ptr<Expression> left;
	std::shared_ptr<Expression> right;
	void accept(IVisitor* visitor) { visitor->visit(this); }
};

class LiteralExpression : public Expression {
public:
	LiteralExpression() {}
	LiteralExpression(Token token) : token(token){}
	Token token;
	void accept(IVisitor* visitor) { visitor->visit(this); }
};

class ClassCreatorExpression : public Expression {
public:
	Token name;
	std::vector<std::shared_ptr<Expression>>arguments;
	void accept(IVisitor* visitor) { visitor->visit(this); }
};

class MethodInvocationExpression : public Expression {
public:
	Token name;
	std::vector<std::shared_ptr<Expression>> arguments;
	void accept(IVisitor* visitor) { visitor->visit(this);}

};

class Statement : public ASTNode {
public:
	virtual ~Statement() {};
};

class BlockStatement : public Statement {
public:
	std::vector<std::shared_ptr<Statement>> stmts;
};

class ExpStatement : public Statement {
public:
	ExpStatement() {}
	ExpStatement(std::shared_ptr<Expression> exp) : expression(exp) {}
	std::shared_ptr<Expression> expression;
};

class IfStatement : public Statement
{
public:
	IfStatement() {}
	std::shared_ptr<Expression> condition;
	std::shared_ptr<Statement> block;
	std::shared_ptr<Statement> elsepart;
};

class WhileStatement : public Statement
{
public:
	std::unique_ptr<Expression> condition;
	std::unique_ptr<Statement> block;
};

class Formal : public ASTNode
{
public:
	Token type;
	Token id;
	std::shared_ptr<Expression> val;
};

class MethodDefinition : public ASTNode
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
	std::vector<std::shared_ptr<MethodDefinition>> methods;
	std::vector<std::unique_ptr<Formal>> fields;
	std::unique_ptr<ClassNode> parent;
	std::string parentname;
};