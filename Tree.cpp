#include <iostream>
#include <fstream>
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

int Tree::Trace(string last, vector< pair<string,string> > now, int k, int stk, fstream& ftree){
	for(int i=0; i<stk; ++i) ftree << "  ";
	ftree << stk+1 << " " << last << endl;
	if(last=="epsilon") return k;
	if(k==(int)now.size()) return k;
	for(auto i : LLtable[last][now[k].first]){
		if(i==now[k].first){
			for(int i=0; i<=stk; ++i) ftree << "  ";
			ftree << stk+2 << " " << i << endl;
			if(now[k].second!=""){
				for(int i=0; i<=stk+1; ++i) ftree << "  ";
				ftree << stk+3 << " " << now[k].second << endl;
			}
			++k;
		}
		else
			k=Trace(i,now,k,stk+1,ftree);
	}
	return k;
}

void Tree::printTree(fstream& ftree){
	Trace(Parse[0].first,Parse,1,0,ftree);
}

