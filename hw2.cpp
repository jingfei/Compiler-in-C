#include <iostream>
#include "Set.h"
#include "Lexer.h"
using namespace std;


int main(){
	freopen("main.c","r",stdin);
	Lexer lexer;
	Set s;
	s.Parse=lexer.getParse();
	freopen("grammar.txt","r",stdin);
	s.input();
	s.findFirst();
	s.findFollow();
	freopen("set.txt","w",stdout);
	s.printFirst();
	printf("\n\n");
	s.printFollow();
	freopen("LLtable.txt","w",stdout);
	s.printLLtable();
	freopen("tree.txt","w",stdout);
	s.Tree();
	return 0;
}


