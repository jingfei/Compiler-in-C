#include <iostream>
#include "Set.h"
using namespace std;


int main(){
	Set s;
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
	return 0;
}


