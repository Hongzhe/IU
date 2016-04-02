#pragma once

#include <string>
#include <memory>
#include <vector>

enum TreeNodeType {
	LITERAL_NODE = 0,
	METHOD_INVOCATION_NODE = 1,
	CREATOR_EXP_NODE = 2,
	TERM_EXP_NODE,
	ARITHMETIC_EXP_NODE,
	RELATION_EXP_NODE,
	BOOLEAN_NODE,
	TYPE_CREATOR_NODE,
	EXPRESSION_NODE,
};


class TreeNode {
public:
	TreeNode() {}
	TreeNode(TreeNodeType type) : type(type) {}
	TreeNodeType type;
	std::string content;
};

class ExpressionTreeNode : public TreeNode {
public:
	ExpressionTreeNode(TreeNodeType type): TreeNode(type){}
	std::shared_ptr<ExpressionTreeNode> left = nullptr;
	std::shared_ptr<ExpressionTreeNode> right = nullptr;
	std::string op;
};


class DeclarationNode :TreeNode
{
public:
	std::string type_id;
	//init value;
};



class MethodInvocationNode : TreeNode
{
public:
	bool isParent;
	std::vector<DeclarationNode> params;
};

class InfixStatmentExp : TreeNode
{
public:
	std::string op;
	//left val   val could be a expression
	//right val
};

class WhileStatementNode : TreeNode
{
	//condition
	//block
};

class IfStatementNode :TreeNode
{
	//condition
	//block
	//else
};



class MethodNode :TreeNode 
{
public:
	std::string returntype;
	//params
	std::vector<DeclarationNode> fields;
};

class ClassTreeNode : public TreeNode 
{

};