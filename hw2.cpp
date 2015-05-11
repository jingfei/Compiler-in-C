#include <iostream>
#include "Compiler.h"
#include "Tree.h"
#include "Set.h"
#include "Lexer.h"
#include "LLTable.h"
using namespace std;


int main(){
	Compiler compiler;
	freopen("grammar.txt","r",stdin);
	compiler.input();

	Lexer lexer;
	freopen("main.c","r",stdin);
	lexer.findLexer();
	compiler.Parse = lexer.getParse();

	Set set(compiler.Gram, compiler.Toks, compiler.Seq);
	set.findFirst();
	set.findFollow();
	freopen("set.txt","w",stdout);
	set.printFirst();
	printf("\n\n");
	set.printFollow();
	compiler.Toks=set.getToks();

	LLTable table(compiler.Gram, compiler.Toks, compiler.Seq);
	table.findLLtable();
	freopen("LLtable.txt","w",stdout);
	table.printLLtable();
	compiler.LLtable = table.getLLtable();

	freopen("tree.txt","w",stdout);
	Tree tree(compiler.LLtable,compiler.Parse);
	tree.printTree();

	return 0;
}


