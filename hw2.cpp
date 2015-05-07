#include <iostream>
#include "Set.h"
using namespace std;


int main(){
	Set s;
	freopen("grammar.txt","r",stdin);
	s.input();
	s.findFirst();
	s.printFirst();
	return 0;
}


