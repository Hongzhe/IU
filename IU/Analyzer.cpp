#include "stdafx.h"
#include "Analyzer.h"

Analyzer::Analyzer() {
	table = new SymbolTable();
	prepare();
}

void Analyzer::prepare()
{	
	//supposed we already have String and Int modules ready
	BlockSymbolTable* stringblock = new BlockSymbolTable();
	BlockSymbolTable* intblock = new BlockSymbolTable();
	table->classMap["String"] = stringblock;
	table->classMap["Int"] = intblock;
}


//start to analyze a class
void Analyzer::analyze(std::shared_ptr<ClassNode> node)
{
	BlockSymbolTable* class_block = table->addClass(node); //add symbols of this class into symbol table;
	if (!class_block) {
		//
	}
}
