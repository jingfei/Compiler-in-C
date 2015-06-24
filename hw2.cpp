#include <iostream>
#include <fstream>
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
	fstream fset;
	fset.open("set.txt", ios::out);
	if(!fset){cerr << "Can't open set.txt!\n"; exit(1);}
	tree.printFirst(fset);
	fset << "\n\n";
	tree.printFollow(fset);
	fset.close();

	freopen("grammar.txt", "r", stdin);
    tree.findLLtable();
	fstream fLLtable;
	fLLtable.open("LLtable.txt", ios::out);
	if(!fLLtable){cerr << "Can't open LLtable.txt!\n"; exit(1);}
	tree.printLLtable(fLLtable);
	fLLtable.close();

	fstream ftree;
	ftree.open("tree.txt", ios::out);
	if(!ftree){cerr << "Can't open tree.txt!\n"; exit(1);}
	tree.printTree(ftree);
	ftree.close();

	//freopen("symbol_table.txt", "w", stdout);
	freopen("tree.txt", "r", stdin);
	SymbolTable symboltable;
	symboltable.findSymbolTable();
	symboltable.printSymbolTable();
//    freopen("screen.txt", "w", stdout);
 	symboltable.genDotDataFile();
	return 0;
}

