#include <iostream>
#include <cstdio>
#include <vector>
#include <cstring>
#include <iomanip>
#include <map>
#include "Set.h"
using namespace std;

void Set::getFirst(struct eachTok &tmpTok){
	if(tmpTok.isTerm) return;
	set <string>::iterator it;
	for(it=tmpTok.First.begin(); it!=tmpTok.First.end(); ++it){
		/* if the child is nonterminal */
		if( !(Toks[*it].isTerm) ){
			getFirst(Toks[*it]);
			/* insert nonterminals & erase this terminal */
			tmpTok.First.insert(Toks[*it].First.begin(),Toks[*it].First.end());
			tmpTok.First.erase(it++);
		}
	}
}

void Set::findFirst(){
	/* add all First() including nonterminal */
	for(auto& i : Toks){
		if(i.second.isTerm) i.second.First.insert(i.first);
		else
			for(auto j : Gram[i.first].content){
				i.second.First.insert(j[0]);
			}
	}
	/* change nonterminal to terminal using recursive */
	for(auto& i : Toks){
		if(i.second.isTerm) continue;
		getFirst(i.second);
	}
}

void Set::printFirst(){
	printf("First\n");
	for(auto i : Toks){
		if(i.second.isTerm) continue; 
		/* print terminals */
		cout << setw(20) << left << i.first << ":";
		for(auto j : i.second.First)
			cout << " " << j;
		printf("\n");
	}
}
/*
void Set::findFollow(){
	Toks["S"].Follow.insert("$");
	for(auto i : Gram){
		for(int j=0; j<i.content.size(); ++j)
			for(int k=1; k<i.content[j].size(); ++k){
				string last=content[j][k-1];
				string now = content[j][k];
				for(auto m : Toks[now].First)
					if(m!="epsilon")
						Toks[last].Follow.insert(m);
			}
	}
}
*/

