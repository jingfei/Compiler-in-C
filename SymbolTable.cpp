#include <iostream>
#include <cstdio>
#include <vector>
#include <map>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include "SymbolTable.h"
using namespace std;

void SymbolTable::findSymbolTable(){
	newScope("0",false);
}

void SymbolTable::newScope(string index, bool isMain){ 
	int n; string gram,new_index="";
	bool stmt=false;
	stack< pair<int,string> > stk;
	scope.push(pair<int,string>(++maxScope,index)); 
	vfuncs.push_back(&mfuncs[index]);
	mfuncs[index].isMain=isMain;
	mfuncs[index].scope=maxScope;
	while(cin >> n >> gram){
		if(gram=="S" || gram=="Program" || gram=="$") continue;
		while(!stk.empty() && stk.top().first>=n) stk.pop();
		stk.push(pair<int,string>(n,gram));
		if(gram[0]=='{') newScope(new_index,isMain);
		else if(gram=="Type"){ // new var (for symbol table)
			stk.pop();
			// which type
			cin >> n >> gram; 
			string type = gram;
			// this is symbol
			cin >> n >> gram;
			// symbol name
			cin >> n >> gram;
			string index = gram + to_string( scope.top().first );
			if(!stk.empty() && stk.top().second=="ParamDecl") index = gram + to_string(maxScope+1); // param
			vSymTable.push_back(&symtable[index]);
			symtable[index].symbol=gram;
			symtable[index].type=type;
			symtable[index].scope = scope.top().first;
			if(!stk.empty() && stk.top().second=="ParamDecl") symtable[index].scope=maxScope+1;  // param
			if(gram=="main") isMain=true;
			// next gram
			cin >> n >> gram;
			if(gram=="Decl") cin >> n >> gram; // next gram
			// judge whether func, ar or not
			if(gram=="FunDecl"){ // func
				symtable[index].func=true;
				symtable[index].func_scope=maxScope+1;
				new_index = index;
			}
			else{  // arr
				cin >> n >> gram;
				if(gram[0]=='[') symtable[index].arr=true;
				// end declare
				int lastn=n;
				do{cin >> n >> gram;}while(n>=lastn);  
			}
		}
		else if(gram[0]=='}'){
			stmt=false;
			break;
		}
		else if(gram=="StmtList"){
			stmt=true;
			mfuncs[index].tree.push_back(pair<int,string>(n,gram));
		}
		else if(stmt){
			mfuncs[index].tree.push_back(pair<int,string>(n,gram));
		}
		if(gram=="if" || gram=="while"){
			new_index = "if"+to_string(maxScope+1);
		}
	}
	scope.pop();
}

void SymbolTable::printSymbolTable(){
	for(auto i : vSymTable){
		cout << setw(5)  << left << i->scope;
		cout << setw(10) << left << i->symbol;
		cout << setw(8)  << left << i->type;
		cout << setw(8)  << left << (i->arr ? "true" : "false");
		cout << setw(8)  << left << (i->func ? "true" : "false"); 
		cout << endl;
	}
}

//void SymbolTable::output(){
//	for(auto i : vfuncs){
////		if(!i->isMain) continue;
//		for(auto j : i->tree){
//			
//		}
//		if( i->tree.begin()->second=="StmtList" ) 
//			Stmt(i->tree.begin()+1,i->tree.end());
////		cout << i->scope << ":\n";
////		int size = i->tree.size();
////		for(int j=0; j<size; ++j){
////			string gram = i->tree[j].second;
////			cout << "\t" << i->tree[j].first << " " << gram << endl;
////		}
//	}
//}
//
//void SymbolTable::StmtList(vector< pair<int,string> >::iterator it,
//					   vector< pair<int,string> >::iterator itend){
//	if(it==itend) return;
//	if(it->second=="Stmt")
//		
//
//}
//
//void SymbolTable::Expr(){
//	if(it->second=="UnaryOp"){
//		++it; string s = it->second;
//		++it; string id = Expr();
//		if(s=="-"){
//			cout << "# Unary minus\n";
//			cout << "lw $t1, " << id << endl;
//			cout << "sub $t1, $zero, $t1\n";
//			cout << "sw $t1, " << id << endl;
//		}
//		else if(s=="!"){
//			cout << "# Not\n";
//			cout << "lw $t1, " << id << endl;
//			cout << "not $t1, $t1\n";
//			cout << "sw $t1, " << id << endl;
//		}
//		return id;
//	}
//	else if(it->second=="num"){
//		++it; string n = it->second;
//		++it; string id = Expr2(n);
//		return id;
//	}
//}
//
//void Expr2(string n){
//	if(it->second=="BinOp"){
//		++it; string op = it->second;
//		++it; string id = Expr();
//		if(op=="+"){
//			cout << "# Add\n";
//			cout << "lw $t1, " << n << endl;
//			cout << "lw $t2, " << id << endl;
//			cout << "add $t1, $t1, $t2\n";
//			cout << "sw $t1, 
//		}
//	}
//}


