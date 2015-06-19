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
	newScope("0");
}

void SymbolTable::newScope(string index){ 
	int n; string gram,new_index="";
	stack< pair<int,string> > stk;
	scope.push(pair<int,string>(++maxScope,index)); 
	while(cin >> n >> gram){
		if(gram=="S" || gram=="Program" || gram=="$") continue;
		while(!stk.empty() && stk.top().first>=n) stk.pop();
		stk.push(pair<int,string>(n,gram));
		if(gram[0]=='{') newScope(new_index);
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
			if(!stk.empty() && stk.top().second=="ParamDecl") 
				symtable[index].scope=maxScope+1;  // param
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
		cout << setw(8) << left << i->func_name;
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
        fp << i->symbol << ":\t";

        if(i->type == "char"){
            if(i->arr)
                fp << ".space\t" << (i->arr_size)*1 << endl;
            else
                fp << ".byte\t\0\n";
        }
        else if(i->type == "int"){
            if(i->arr)
                fp << ".space\t" << (i->arr_size)*4 << endl;
            else
                fp << ".word\t\0\n";
        }
        else if(i->type == "float"){
            if(i->arr)
                fp << ".space\t" << (i->arr_size)*4 << endl;
            else
                fp << ".float\t\0\n";
        }
        else if(i->type == "double"){
            if(i->arr)
                fp << ".space\t" << (i->arr_size)*8 << endl;
            else
                fp << ".double\t\0\n";
        }
    }
    fp.close();
}

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
			ftext << "\t# Unary minus\n";
			ftext << "\tlw $t1, " << id << endl;
			ftext << "\tsub $t1, $zero, $t1\n";
			ftext << "\tsw $t1, " << id << endl;
		}
		else if(s=="!"){
			ftext << "\t# Not\n";
			ftext << "\tlw $t1, " << id << endl;
			ftext << "\tnot $t1, $t1\n";
			ftext << "\tsw $t1, " << id << endl;
		}
		return id;
	}
	else if(gram=="num"){
		cin >> n >> gram; string num = gram;
		cin >> n >> gram; string id = Expr2(num,true);
		return id;
	}
	else if(gram=="("){
		cin >> n >> gram; string id = Expr(); 
		cin >> n >> gram; // )
		cin >> n >> gram; id = Expr2(id,false);
		return id;
	}
	else if(gram=="id"){
		cin >> n >> gram; string id = gram;
		cin >> n >> gram; id = ExprIdTail(id);
		return id;
	}
	return "epsilon";
}

string SymbolTable::Expr2(string pre, bool isNum){
	int n; string gram; cin >> n >> gram;
	if(gram=="BinOp"){
		cin >> n >> gram; string op = gram;
		cin >> n >> gram; string id = Expr();
		if(isNum){
			ftext << "\t# move num\n";
			ftext << "\tli $t4, " << pre << endl;
			pre="$t4";
		}
		if(op=="+"){
			ftext << "\t# Add\n";
			ftext << "\tlw $t1, " << pre << endl;
			ftext << "\tlw $t2, " << id << endl;
			ftext << "\tadd $t3, $t1, $t2\n";
		}
		else if(op=="-"){
			ftext << "\t# Sub\n";
			ftext << "\tlw $t1, " << pre << endl;
			ftext << "\tlw $t2, " << id << endl;
			ftext << "\tsub $t3, $t1, $t2\n"; 
		}
		else if(op=="*"){
			ftext << "\t# Mult\n";
			ftext << "\tlw $t1, " << pre << endl;
			ftext << "\tlw $t2, " << id << endl;
			ftext << "\tmul $t3, $t1, $t2\n";
		}
		else if(op=="/"){
			ftext << "\t# Div\n";
			ftext << "\tlw $t1, " << pre << endl;
			ftext << "\tlw $t2, " << id << endl;
			ftext << "\tdiv $t3, $t1, $t2\n";
		}
		return "$t3";
	}
	else if(gram=="epsilon"){
		if(isNum){
			ftext << "\t# move num\n";
			ftext << "\tli $t4, " << pre << endl;
			pre="$t4";
		}
		return pre;
	}
	return "epsilon";
}

string SymbolTable::ExprIdTail(string pre){
	int n; string gram; cin >> n >> gram;
	if(gram=="Expr'"){
		string id = Expr2(pre, false);
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
		ftext << "\t# Equal\n";
		ftext << "\tlw $t1, " << id << endl;
		ftext << "\tsw $t1, " << pre << endl;
	}
	return "epsilon";
}

