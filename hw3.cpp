#include <iostream>
#include <fstream>
#include "Compiler.h"
#include "Tree.h"
#include "Set.h"
#include "Lexer.h"
#include "LLTable.h"
#include "SymbolTable.h"
using namespace std;

int main(int argc, char* argv[]){
	if(argc<2){
		cout << "Please put input c file in argv\n";
		exit(1);
	}
	Tree tree;
	freopen(argv[1],"r",stdin);
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
	SymbolTable symboltable(argv[1]);
	symboltable.findSymbolTable();
	symboltable.printSymbolTable();
 	symboltable.genDotDataFile();
	return 0;
}

