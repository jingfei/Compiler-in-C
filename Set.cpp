#include <iostream>
#include <cstdio>
#include <vector>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <map>
#include "Set.h"
using namespace std;

void Set::insertLLtable(string row,string col,string head){
	if(col=="epsilon") return;
	map<string, vector<string> >::iterator it = LLtable[row].find(col);
	if(it!=LLtable[row].end()) return;
	for(auto i : Gram[row].content)
		if(i[0]==head)
			for(auto j : i)
				LLtable[row][col].push_back(j);
	if(col=="Program") LLtable[row][col].push_back("$");
}

void Set::getFirst(struct eachTok &tmpTok){
	if(tmpTok.isTerm) return;
	set <string>::iterator it;
	for(it=tmpTok.First.begin(); it!=tmpTok.First.end(); ++it){
		/* if the child is nonterminal */
		if( !(Toks[*it].isTerm) ){
			getFirst(Toks[*it]);
			/* insert nonterminals */
			tmpTok.First.insert(Toks[*it].First.begin(),Toks[*it].First.end());
			/* insert into LLtable */
			for(auto i : Toks[*it].First)
				insertLLtable(tmpTok.name,i,*it);
			/* erase this terminal */
			tmpTok.First.erase(it--);
		}
		/* insert nonTerm,Term into LLtable */
		else
			insertLLtable(tmpTok.name,*it,*it);
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

void Set::findFollow(){
	/* step 1 */
	Toks["S"].Follow.insert("$");
	Toks["Program"].Follow.insert("$");
	/* step 2 */
	for(auto i : Gram){
		for(int j=0; j<i.second.content.size(); ++j)
			for(int k=1; k<i.second.content[j].size(); ++k){
				string last= i.second.content[j][k-1];
				string now = i.second.content[j][k];
				for(auto m : Toks[now].First)
					if(m!="epsilon")
						Toks[last].Follow.insert(m);
			}
	}
	/* step 3 */
	for(auto l : Gram)
		for(auto i : Gram){
			for(int j=0; j<i.second.content.size(); ++j){
				int k=i.second.content[j].size()-1;
				string now = i.second.content[j][k];
				Toks[now].Follow.insert(Toks[i.first].Follow.begin(),
										Toks[i.first].Follow.end());
				for( ; k; --k){
					string last= i.second.content[j][k-1];
					now = i.second.content[j][k];
					/* find epsilon */
					set<string>::iterator it = Toks[now].First.find("epsilon");
					if(it==Toks[now].First.end()) break;
					Toks[last].Follow.insert(Toks[i.first].Follow.begin(),
											 Toks[i.first].Follow.end());
				}
			}
		}
	/* insert LLtable */
	for(auto i : Gram){
		bool findEpsilon=false;
		for(auto j : i.second.content){
			if(find(j.begin(),j.end(),"epsilon")!=j.end()){
				findEpsilon=true;
				break;
			}
		}
		if(findEpsilon){
			for(auto j : Toks[i.first].Follow)
				LLtable[i.first][j].push_back("epsilon");
		}
	}
}

void Set::printFollow(){
	printf("Follow\n");
	for(auto i : Toks){
		if(i.second.isTerm) continue; 
		/* print terminals */
		cout << setw(20) << left << i.first << ":";
		for(auto j : i.second.Follow)
			cout << " " << j;
		printf("\n");
	}
}

void Set::printLLtable(){
	for(auto i : LLtable)
		for(auto j : i.second){
			cout << setw(20) << left << i.first
				 << setw(20) << left << j.first;
			for(auto k : j.second)
				cout << k << " ";
			cout << endl;
		}
}

