#pragma once

#include "stdafx.h"
#include "Lexer.h"
#include <vector>
#include <memory>
#include <string>
#include <iostream>

class BinaryExpression;
class LiteralExpression;
class MethodInvocationExpression;
class ClassCreatorExpression;
class PranExpression;
class VariableDeclareExpression;
class IfStatement;
class WhileStatement;
class MethodDefinition;
class Formal;
class ClassNode;
class BlockStatement;
class ExpStatement;

//visitor pattern

class IVisitor
{
public:
	virtual void visit(std::shared_ptr<BinaryExpression> node) = 0;
	virtual void visit(std::shared_ptr<LiteralExpression> node) = 0;
	virtual void visit(std::shared_ptr<MethodInvocationExpression> node) = 0;
	virtual void visit(std::shared_ptr<ClassCreatorExpression> node) = 0;
	virtual void visit(std::shared_ptr<PranExpression> node) = 0;
	virtual void visit(std::shared_ptr<Formal> node) = 0;
	virtual void visit(std::shared_ptr<IfStatement> node) = 0;
	virtual void visit(std::shared_ptr<WhileStatement> node) = 0;
	virtual void visit(std::shared_ptr<MethodDefinition> node) = 0;
	virtual void visit(std::shared_ptr<ClassNode> node) = 0;
	virtual void visit(std::shared_ptr<BlockStatement> node) = 0;
	virtual void visit(std::shared_ptr<ExpStatement> node) = 0;
	virtual void visit(std::shared_ptr<VariableDeclareExpression> node) = 0;
};


enum ASTNodeType {
	LITERAL_EXP, PRAN_EXP, BINARY_EXP, METHOD_INVOC_EXP, CREATOR_EXP,VAR_DECL_EXP,
	IF_STMT, WHILE_STMT, METHOD_DEFINE_STMT,CLASS_NODE, FORMAL, BLOCK_STMT,
	EXP_STMT,RETURN_STMT
};

class ASTNode {
public:
	unsigned int lineno;
	ASTNodeType node_type;
	virtual void accept(IVisitor* visitor) = 0;
};

class Expression : public ASTNode {
public:
	virtual ~Expression() {}
};


class PranExpression : public Expression {
public:
	std::shared_ptr<Expression> exp;
	void accept(IVisitor* visitor) { visitor->visit(std::shared_ptr<PranExpression>(this)); }
};

class BinaryExpression : public Expression {
public:
	BinaryExpression(Token op, std::shared_ptr<Expression> l,
		std::shared_ptr<Expression> r) : op(op), left(l), right(r) {}
	BinaryExpression() {}
	Token op;
	std::shared_ptr<Expression> left;
	std::shared_ptr<Expression> right;
	void accept(IVisitor* visitor) { visitor->visit(std::shared_ptr<BinaryExpression>(this)); }
};

class VariableDeclareExpression : public Expression
{
public:
	VariableDeclareExpression(Token id, Token type): id(id), type(type) {}
	VariableDeclareExpression() {}
	Token id;
	Token type;
	void accept(IVisitor* visitor) { visitor->visit(std::shared_ptr<VariableDeclareExpression>(this)); }
};

class LiteralExpression : public Expression {
public:
	LiteralExpression() {}
	LiteralExpression(Token token) : token(token){}
	Token token;
	void accept(IVisitor* visitor) { visitor->visit(std::shared_ptr<LiteralExpression>(this)); }
};

class ClassCreatorExpression : public Expression {
public:
	Token name;
	std::vector<std::shared_ptr<Expression>>arguments;
	void accept(IVisitor* visitor) { visitor->visit(std::shared_ptr<ClassCreatorExpression>(this)); }
};

class MethodInvocationExpression : public Expression {
public:
	Token name;
	std::vector<std::shared_ptr<Expression>> arguments;
	void accept(IVisitor* visitor) { visitor->visit(std::shared_ptr<MethodInvocationExpression>(this));}
};

class Statement : public ASTNode {
public:
	virtual ~Statement() {};
};

class BlockStatement : public Statement {
public:
	std::vector<std::shared_ptr<Statement>> stmts;
	void accept(IVisitor* visitor) { visitor->visit(std::shared_ptr<BlockStatement>(this)); }
};

class ExpStatement : public Statement {
public:
	ExpStatement() {}
	ExpStatement(std::shared_ptr<Expression> exp) : expression(exp) {}
	std::shared_ptr<Expression> expression;
	void accept(IVisitor* visitor) { visitor->visit(std::shared_ptr<ExpStatement>(this)); }
};

class IfStatement : public Statement
{
public:
	IfStatement() {}
	std::shared_ptr<Expression> condition;
	std::shared_ptr<BlockStatement> block;
	std::shared_ptr<BlockStatement> elsepart;
	void accept(IVisitor* visitor) { visitor->visit(std::shared_ptr<IfStatement>(this)); }
};

class WhileStatement : public Statement
{
public:
	std::shared_ptr<Expression> condition;
	std::shared_ptr<BlockStatement> block;
	void accept(IVisitor* visitor) { visitor->visit(std::shared_ptr<WhileStatement>(this)); }
};

class Formal : public ASTNode
{
public:
	Token type;
	Token id;
	std::shared_ptr<Expression> val;
	void accept(IVisitor* visitor) { visitor->visit(std::shared_ptr<Formal>(this)); }
};

class MethodDefinition : public ASTNode
{
public:
	Token returntype;
	Token name;
	std::vector<std::shared_ptr<Formal>> arguments;
	std::shared_ptr<BlockStatement> block;
	void accept(IVisitor* visitor) { visitor->visit(std::shared_ptr<MethodDefinition>(this)); }
};

class ClassNode : public ASTNode
{
public:
	Token classname;
	std::vector<std::shared_ptr<MethodDefinition>> methods;
	std::vector<std::shared_ptr<Formal>> fields;
	std::unique_ptr<ClassNode> parent;
	std::string parentname;
	void accept(IVisitor* visitor) { visitor->visit(std::shared_ptr<ClassNode>(this)); }
};

class TreePrinter : public IVisitor
{
public:
	void visit(std::shared_ptr<Expression> node) {
		if (!node) return;
		switch (node->node_type) {
		case LITERAL_EXP:
			visit(std::dynamic_pointer_cast<LiteralExpression>(node));
			break;
		case FORMAL:
			visit(std::dynamic_pointer_cast<Formal>(node));
			break;
		case CREATOR_EXP:
			visit(std::dynamic_pointer_cast<ClassCreatorExpression>(node));
			break;
		case METHOD_INVOC_EXP:
			visit(std::dynamic_pointer_cast<MethodInvocationExpression>(node));
			break;
		case PRAN_EXP:
			visit(std::dynamic_pointer_cast<PranExpression>(node));
			break;
		case BINARY_EXP:
			visit(std::dynamic_pointer_cast<BinaryExpression>(node));
			break;
		}
	}

	void visit(std::shared_ptr<PranExpression> node)
	{
		std::cout << "( ";
		visit(node->exp);
		std::cout << " )" << std::endl;
	}

	void visit(std::shared_ptr<BinaryExpression> node)
	{
		visit(node->left);
		std::cout << " " << node->op.lexem << " ";
		visit(node->right);
	}

	void visit(std::shared_ptr<LiteralExpression> node) {
		std::cout << node->token.lexem << " ";
	}

	void visit(std::shared_ptr<Formal> node)
	{
		std::cout << node->type.lexem << " " << node->id.lexem;
		if (node->val) {
			visit(node->val);
		}
	}

	void visit(std::shared_ptr<ClassCreatorExpression> node)
	{
		std::cout << "new " << node->name.lexem << "( ";
		std::vector<std::shared_ptr<Expression>> arguments = node->arguments;
		for (int i = 0; i < (int)arguments.size(); i++) {
			visit(arguments[i]);
			std::cout << " ";
		}
		std::cout << ")";
	}

	void visit(std::shared_ptr<MethodInvocationExpression> node) {
		std::cout << node->name.lexem << " (";
		std::vector<std::shared_ptr<Expression>> argumnets = node->arguments;
		for (int i = 0; i < (int)argumnets.size(); i++)
		{
			visit(argumnets[i]);
		}
		std::cout << ")" << std::endl;
	}
	void visit(std::shared_ptr<Statement> node)
	{
		if (!node) return;
		switch (node->node_type) 
		{
		case IF_STMT:
			visit(std::dynamic_pointer_cast<IfStatement>(node));
			break;
		case WHILE_STMT:
			visit(std::dynamic_pointer_cast<WhileStatement>(node));
			break;
		case METHOD_DEFINE_STMT:
			visit(std::dynamic_pointer_cast<MethodDefinition>(node));
			break;
		case BLOCK_STMT:
			visit(std::dynamic_pointer_cast<BlockStatement>(node));
			break;
		case EXP_STMT:
			visit(std::dynamic_pointer_cast<ExpStatement>(node));
			break;
		case RETURN_STMT:
			visit(std::dynamic_pointer_cast<ExpStatement>(node));
			break;
		}
	}

	void visit(std::shared_ptr<IfStatement> node) 
	{
		std::cout << "if (" ;
		visit(node->condition);
		std::cout << " )";
		visit(node->block);
		if (node->elsepart) {
			std::cout << "else ";
			visit(node->elsepart);
		}
	}

	void visit(std::shared_ptr<WhileStatement> node) 
	{
		std::cout << "while (";
		visit(node->condition);
		std::cout << " )";
		visit(node->block);
	}

	void visit(std::shared_ptr<MethodDefinition> node) 
	{
		std::cout << node->returntype.lexem << " ";
		std::cout << node->name.lexem << " ги";
		auto arguments = node->arguments;
		for (int i = 0; i < (int)arguments.size(); i++) {
			visit(arguments[i]);
		}
		std::cout << ")";
		visit(node->block);
	}

	void visit(std::shared_ptr<BlockStatement> node) 
	{
		std::cout << "{ " << std::endl;
		auto stmts = node->stmts;
		for (int i = 0; i < (int)stmts.size(); i++) {
			visit(stmts[i]);
		}
		std::cout << "}" << std::endl;
	}
	
	void visit(std::shared_ptr<ExpStatement> node) 
	{
		if (node->node_type == RETURN_STMT) {
			std::cout << "return ";
		}
		visit(node->expression);
	}

	void visit(std::shared_ptr<VariableDeclareExpression> node)
	{
		std::cout << node->type.lexem << " " << node->id.lexem;
	}

	void visit(std::shared_ptr<ClassNode> node) 
	{
		std::cout << "class " << node->classname.lexem;
		if (node->parent) {
			std::cout << "inherits " << node->parent->classname.lexem;
		}
		std::cout << "{ ";
		auto fields = node->fields;
		for (int i = 0; i < (int)fields.size(); i++) {
			visit(fields[i]);
		}
		std::cout << std::endl;
		auto methods = node->methods;
		for (int j = 0; j < (int)methods.size(); j++) {
			visit(methods[j]);
			std::cout << std::endl;
		}
		std::cout << "}" << std::endl;
	}
};