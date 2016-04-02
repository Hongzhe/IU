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
	ARGUMENTS_NODE,
	EXPRESSION_NODE,
	FIELD_DECLARATOR_NODE,
	FIELD_NODE,
	MEMBER_METHOD_NDOE,
	CONSTRUCTOR_NODE
};


class TreeNode {
public:
	TreeNode() {}
	TreeNode(TreeNodeType type) : type(type) {}
	TreeNodeType type;
	std::string content;
	//std::shared_ptr<TreeNode> next;
};

class ExpressionTreeNode : public TreeNode {
public:
	ExpressionTreeNode(TreeNodeType type): TreeNode(type){}
	std::shared_ptr<ExpressionTreeNode> left = nullptr;
	std::shared_ptr<ExpressionTreeNode> right = nullptr;
	std::string op;
};


class StatementNode : public TreeNode
{
public:
	std::vector<std::shared_ptr<TreeNode>> statements;
};

class ConditionStatementNode : public TreeNode
{
public:
	std::shared_ptr<ExpressionTreeNode> condition;
	std::shared_ptr<StatementNode> body;
	std::shared_ptr<StatementNode> elsebody;
};

class DeclarationNode : public TreeNode
{
public:
	DeclarationNode() { type = FIELD_DECLARATOR_NODE; }
	std::string type_id;
	std::shared_ptr<ExpressionTreeNode> init;
	std::shared_ptr<DeclarationNode> next;
};

class FeatureTreeNode : public TreeNode
{
public:
	std::shared_ptr<DeclarationNode> declaration;
	std::shared_ptr<DeclarationNode> parameters;
	std::shared_ptr<StatementNode> statements;
};

class ClassTreeNode : public TreeNode 
{
public:
	std::string id;
	std::string parent;
	std::vector<FeatureTreeNode> features;
};