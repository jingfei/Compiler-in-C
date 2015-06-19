#include <iostream>
#include <cstdio>
#include <vector>
#include <map>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <fstream>
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
//	vfuncs.push_back(&mfuncs[index]);
//	mfuncs[index].isMain=isMain;
//	mfuncs[index].scope=maxScope;
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
			// read Stmt
			cin >> n >> gram;
			if(gram=="Stmt") Stmt();
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

void SymbolTable::genDotDataFile(){
    fstream fp;
    fp.open(".data", ios::out);
    for(auto i : vSymTable){
        fp << i->symbol << i->scope << ":\t";

        if(i->type == "int")
            fp << ".word\t0\n";
        else if(i->type == "float")
            fp << ".float\t0.0\n";
        else if(i->type == "double")
            fp << ".double\t0.0\n";
        else if(i->type == "char")
            fp << ".byte\t\0\n";
    }
    fp.close();
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

void SymbolTable::Stmt(){
	int n; string gram; cin >> n >> gram;
	if(gram==";") return;
	else if(gram=="Expr"){
		Expr();
		cin >> n >> gram; //;
	}
//	else if(gram=="return"){
//	}
//	else if(gram=="break"){
//		cin >> n >> gram; //;
//	}
//	else if(gram=="if"){
//	}
//	else if(gram=="while"){
//	}
//	else if(gram=="Block"){
//	
//	}
	else if(gram=="print"){
		cin >> n >> gram; string id = gram;
		cin >> n >> gram; //;
		ftext << "\t# print\n";
		ftext << "\tli $v0, 1\n";
		ftext << "\tadd $a0, " << id << ", $zero\n";
		ftext << "\tsyscall\n";
		ftext << "\tjr $ra\n";
	}
}

string SymbolTable::Expr(){
	int n; string gram; cin >> n >> gram;
	if(gram=="UnaryOp"){
		cin >> n >> gram; string s = gram;
		cin >> n >> gram; string id = Expr();
		if(s=="-"){
			ftext << "\t" << "# Unary minus\n";
			ftext << "\t" << "lw $t1, " << id << endl;
			ftext << "\t" << "sub $t1, $zero, $t1\n";
			ftext << "\t" << "sw $t1, " << id << endl;
		}
		else if(s=="!"){
			ftext << "\t" << "# Not\n";
			ftext << "\t" << "lw $t1, " << id << endl;
			ftext << "\t" << "not $t1, $t1\n";
			ftext << "\t" << "sw $t1, " << id << endl;
		}
		return id;
	}
	else if(gram=="num"){
		cin >> n >> gram; string num = gram;
		cin >> n >> gram; string id = Expr2(num);
		return id;
	}
	else if(gram=="("){
		cin >> n >> gram; string id = Expr(); 
		cin >> n >> gram; // )
		cin >> n >> gram; id = Expr2(id);
		return id;
	}
	else if(gram=="id"){
		cin >> n >> gram; string id = gram;
		cin >> n >> gram; id = ExprIdTail(id);
		return id;
	}
	return "epsilon";
}

string SymbolTable::Expr2(string pre){
	int n; string gram; cin >> n >> gram;
	if(gram=="BinOp"){
		cin >> n >> gram; string op = gram;
		cin >> n >> gram; string id = Expr();
		if(op=="+"){
			ftext << "\t" << "# Add\n";
			ftext << "\t" << "lw $t1, " << pre << endl;
			ftext << "\t" << "lw $t2, " << id << endl;
			ftext << "\t" << "add $t3, $t1, $t2\n";
		}
		else if(op=="-"){
			ftext << "\t" << "# Sub\n";
			ftext << "\t" << "lw $t1, " << pre << endl;
			ftext << "\t" << "lw $t2, " << id << endl;
			ftext << "\t" << "sub $t3, $t1, $t2\n"; 
		}
		else if(op=="*"){
			ftext << "\t" << "# Mult\n";
			ftext << "\t" << "lw $t1, " << pre << endl;
			ftext << "\t" << "lw $t2, " << id << endl;
			ftext << "\t" << "mul $t3, $t1, $t2\n";
		}
		else if(op=="/"){
			ftext << "\t" << "# Div\n";
			ftext << "\t" << "lw $t1, " << pre << endl;
			ftext << "\t" << "lw $t2, " << id << endl;
			ftext << "\t" << "div $t3, $t1, $t2\n";
		}
		return "$t3";
	}
	else if(gram=="epsilon") return pre;
	return "epsilon";
}

string SymbolTable::ExprIdTail(string pre){
	int n; string gram; cin >> n >> gram;
	if(gram=="Expr'"){
		string id = Expr2(pre);
		return id;
	}
//	else if(gram=="("){
//		cin >> n >> gram; 
//	}
//	else if(gram=="["){
//		cin >> n >> gram; string id = Expr();
//		cin >> n >> gram; // ]
//		// get pre[id]
//	}
	else if(gram=="="){
		cin >> n >> gram; string id = Expr();
		ftext << "\t" << "# Equal\n";
		ftext << "\t" << "lw $t1, " << id << endl;
		ftext << "\t" << "sw $t1, " << pre << endl;
	}
	return "epsilon";
}

