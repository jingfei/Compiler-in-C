#include <iostream>
#include <cstdio>
#include <vector>
#include <map>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include "LLTable.h"
using namespace std;

//LLTable::LLTable(map <string,Grammar> Gram,	
//					map <string,eachTok> Toks,
//					vector <string> Seq){
//	this->Gram = Gram;
//	this->Toks = Toks;
//	this->Seq = Seq;
//}

map <string, map<string, vector<string> > > LLTable::getLLtable(){
	return LLtable;
}

void LLTable::insertLLtable(string row,string col,string head){
	if(col=="epsilon"){
		for(auto i : Toks[row].Follow)
			insertLLtable(row,i,head);
	}
	else if(Toks[col].isTerm){
		for(auto i : Gram[row].content)
			if(i[0]==head)
				for(auto j : i)
					LLtable[row][col].push_back(j);
	}
	else{
		for(auto i : Toks[col].First)
			insertLLtable(row,i,head);
	}
}

void LLTable::findLLtable(){
	for(auto i : Gram){
		for(auto j : i.second.content){
			string first=j[0];
			insertLLtable(i.first,first,first);
		}
	}
}

void LLTable::printLLtable(){
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

