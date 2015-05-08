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
	Toks["S"].First.insert("$");
	Toks["S"].First.erase("epsilon");
//	for(auto& i : Toks){
//		if(i.second.isTerm) continue;
//		else  
//			for(auto j : Gram[i.first].content){
//				string last=j[j.size()-1];
//				for(auto k : j){
//					if(k==last) 
//						Toks[i.first].First.insert(Toks[k].First.begin(), 
//													Toks[k].First.end() );
//					set<string>::iterator it=Toks[k].First.find("epsilon");
//					if(it==Toks[k].First.end() && !Toks[k].isTerm) break;
//					Toks[i.first].First.insert("epsilon");
//				}
//			}
//	}
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
	LLtable["Program"]["$"].push_back("DeclList");
	LLtable["S"]["$"].push_back("Program");
	LLtable["S"]["$"].push_back("$");
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

void Set::printLLtable(){
	cout << "S\n";
	for(auto n : Seq)
		for(auto j : LLtable[n]){
			cout << setw(20) << left << n
				 << setw(20) << left << j.first;
			for(auto k : j.second)
				cout << k << " ";
			cout << endl;
		}
}

int Set::Trace(string last, vector< pair<string,string> > now, int k, int stk){
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
//	for(int i=0; i<stk; ++i) printf("  ");
//	cout << stk+1 << " " << last << endl;
	return k;
}

void Set::Tree(){
	Trace(Parse[0].first,Parse,1,0);
}

