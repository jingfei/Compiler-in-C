#include <iostream>
#include <cstdio>
#include <vector>
#include <map>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include "Tree.h"
using namespace std;

//Tree::Tree(map <string, map<string, vector<string> > > LLtable, 
//			vector< pair<string,string> > Parse){
//	this->LLtable = LLtable;
//	this->Parse = Parse;
//}

int Tree::Trace(string last, vector< pair<string,string> > now, int k, int stk){
	for(int i=0; i<stk; ++i) printf("  ");
	cout << stk+1 << " " << last << endl;
	if(last=="epsilon") return k;
	if(k==(int)now.size()) return k;
	for(auto i : LLtable[last][now[k].first]){
		if(i==now[k].first){
			for(int i=0; i<=stk; ++i) printf("  ");
			cout << stk+2 << " " << i << endl;
			if(now[k].second!=""){
				for(int i=0; i<=stk+1; ++i) printf("  ");
				cout << stk+3 << " " << now[k].second << endl;
			}
			++k;
		}
		else
			k=Trace(i,now,k,stk+1);
	}
	return k;
}

void Tree::printTree(){
	Trace(Parse[0].first,Parse,1,0);
}

