#include <iostream>
#include "Compiler.h"
#include "Tree.h"
#include "Set.h"
#include "Lexer.h"
#include "LLTable.h"
#include "SymbolTable.h"
using namespace std;

int main(){
	Tree tree;
	freopen("main.c","r",stdin);
	tree.findLexer();

	freopen("grammar.txt","r",stdin);
	tree.input();
	tree.findFirst();
	tree.findFollow();
	freopen("set.txt","w",stdout);
	tree.printFirst();
	printf("\n\n");
	tree.printFollow();

	freopen("grammar.txt", "r", stdin);
	tree.findLLtable();
	freopen("LLtable.txt","w",stdout);
	tree.printLLtable();

	freopen("tree.txt","w",stdout);
	tree.printTree();

	freopen("symbol_table.txt", "w", stdout);
	freopen("tree.txt", "r", stdin);
	SymbolTable symboltable;
	symboltable.findSymbolTable();
	symboltable.printSymbolTable();
 	symboltable.genDotDataFile();
	return 0;
}

