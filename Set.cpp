#include <iostream>
#include <cstdio>
#include <vector>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <map>
#include "Set.h"
using namespace std;

Set::Set(map <string,Grammar> Gram, 
			map <string,eachTok> Toks, 
			vector <string> Seq){
	this->Gram = Gram;
	this->Toks = Toks;
	this->Seq = Seq;
}

map <string,Compiler::eachTok> Set::getToks(){
	return Toks;
}

void Set::getFirst(struct eachTok &tmpTok){
	if(tmpTok.isTerm) return;
	set <string>::iterator it;
	for(it=tmpTok.First.begin(); it!=tmpTok.First.end(); ++it){
		/* if the child is nonterminal */
		if( !(Toks[*it].isTerm) ){
			getFirst(Toks[*it]);
			/* insert nonterminals */
			for(auto i : Toks[*it].First)
				if(i!="epsilon")
					tmpTok.First.insert(i);
			/* erase this terminal */
			tmpTok.First.erase(it--);
		}
	}
}

void Set::findFirst(){
	/* add all First() including nonterminal */
	for(auto& i : Toks){
		if(i.second.isTerm) i.second.First.insert(i.first);  //term itself
		else  //term add first child
			for(auto j : Gram[i.first].content){
				i.second.First.insert(j[0]);
			}
	}
	/* change nonterminal to terminal using recursive */
	for(auto& i : Toks){
		if(i.second.isTerm) continue;
		getFirst(i.second);
	}
	/* step 2 & 3 */
	for(auto i : Gram){
		if(Toks[i.first].First.find("epsilon")==Toks[i.first].First.end()){
			for(auto j : i.second.content){
				bool allHasEpsilon=true;
				bool exceptLast=true;
				for(auto k : j)
					if(Toks[k].First.find("epsilon")==Toks[k].First.end()){
						allHasEpsilon=false;
						if(k!= *(j.end()-1) ) exceptLast=false;
						break;
					}
				if(exceptLast)
					Toks[i.first].First.insert( Toks[*(j.end()-1)].First.begin(),
																			Toks[*(j.end()-1)].First.end() );
				if(allHasEpsilon){
					Toks[i.first].First.insert("epsilon");
					break;
				}
			}
		}
	}
}

void Set::printFirst(){
	printf("First\n");
	for(auto n : Seq){
		struct eachTok i = Toks[n];
		if(i.isTerm) continue; 
		/* print terminals */
		cout << setw(20) << left << i.name << ":";
		for(auto j : i.First)
			cout << " " << j;
		printf("\n");
	}
}

void Set::findFollow(){
	/* step 1 */
	Toks["S"].Follow.insert("$");
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
}

void Set::printFollow(){
	printf("Follow\n");
	for(auto n : Seq){
		struct eachTok i = Toks[n];
		if(i.isTerm) continue; 
		/* print terminals */
		cout << setw(20) << left << i.name << ":";
		for(auto j : i.Follow)
			cout << " " << j;
		printf("\n");
	}
}

