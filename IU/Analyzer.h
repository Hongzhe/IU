#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "ast.h"
#include "SymbolTable.h"
/*symbol table entry*/


class Analyzer
{
public:
	class AnalyzerVisitor : public IVisitor {
	private:
		SymbolTable* table;
	public:
		AnalyzerVisitor(Analyzer& analyzer): table(analyzer.table){}
		void visit(std::shared_ptr<ClassNode>);
		void visit(std::shared_ptr<Formal>);
		void visit(std::shared_ptr<MethodDefinition>);
		void visit(std::shared_ptr<IfStatement>);
		void visit(std::shared_ptr<WhileStatement>);
		void visit(std::shared_ptr<BlockStatement>);
		void visit(std::shared_ptr<ExpStatement>);
		void visit(std::shared_ptr<Expression>);
		void visit(std::shared_ptr<BinaryExpression>);
	};
	SymbolTable* table;
	Analyzer();
	void prepare();
	void analyze(std::shared_ptr<ClassNode>);
	
};



