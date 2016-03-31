#pragma once

#include <string>
#include <memory>
#include <vector>

class TreeNode {
public:
	TreeNode() {}
	std::string name;
};

class ExpressionNode : public TreeNode {
	
};

class InfixExpressionNode : public TreeNode {
public:
	std::string op;
	std::shared_ptr<ExpressionNode> leftval;
	std::shared_ptr<ExpressionNode> rightval;
};

class LiteralTreeNode : TreeNode
{
public:
	std::string content;
};

class DeclarationNode :TreeNode
{
public:
	std::string type_id;
	//init value;
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