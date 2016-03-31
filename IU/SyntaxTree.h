#pragma once

#include <string>
#include <memory>
#include <vector>

class TreeNode {
public:
	TreeNode(){}
	std::string name;
};

class LiteralTreeNode : TreeNode
{
public:
	std::string content;
};

class NumericNode : TreeNode
{
public:
	std::string content;
};

class identifierNode : TreeNode
{
public:
	std::string content;
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

class DeclarationNode :TreeNode
{
public:
	std::string type_id;
	//init value;
};

class MethodNode :TreeNode 
{
public:
	std::string returntype;
	//params
	std::vector<DeclarationNode> fields;
};

class ClassTreeNode : TreeNode 
{

};