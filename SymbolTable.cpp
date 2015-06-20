#include <iostream>
#include <cstdio>
#include <vector>
#include <map>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <cctype>
#include "SymbolTable.h"
using namespace std;

void SymbolTable::findSymbolTable(){
	ftext << ".text\n";
	newScope("0",false);
}

void SymbolTable::newScope(string index, bool moveStk){ 
	int n; string gram,new_index="";
	stack< pair<int,string> > stk;
	scope.push(pair<int,string>(++maxScope,index)); 
	while(cin >> n >> gram){
		if(gram=="S" || gram=="Program" || gram=="$") continue;
		while(!stk.empty() && stk.top().first>=n) stk.pop();
		stk.push(pair<int,string>(n,gram));
		if(gram[0]=='{') newScope(new_index, moveStk);
		else if(gram=="Type"){ // new var (for symbol table)
			stk.pop();
			// which type
			cin >> n >> gram; 
			string type = gram;
			// this is symbol
			cin >> n >> gram;
			// symbol name
			cin >> n >> gram;
			string index = gram; // + to_string( scope.top().first );
			if(!stk.empty() && stk.top().second=="ParamDecl") index = gram; // + to_string(maxScope+1); // param
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
				ftext << symtable[index].symbol << ":\n";
				if(symtable[index].symbol!="main"){
					moveStk=true;
					ftext << "\tsub $sp, $sp, 16\n";
					ftext << "\tsw $t1, 4($sp)\n";
					ftext << "\tsw $t2, 8($sp)\n";
					ftext << "\tsw $t3, 12($sp)\n";
					ftext << "\tsw $t4, 16($sp)\n";
				}
				else moveStk=false;
			}
			else{  // arr
				cin >> n >> gram;
				if(gram[0]=='[') symtable[index].arr=true;
				// end declare
				int lastn=n;
				do{
					cin >> n >> gram;
					if(gram=="num"){
						cin >> n >> n;
						symtable[index].arr_size=n;
					}
				}while(n>=lastn);  
			}
		}
		else if(gram[0]=='}'){
			if(moveStk){
				moveStk=false;
				ftext << "\tlw $t1, 4($sp)\n";
				ftext << "\tlw $t2, 8($sp)\n";
				ftext << "\tlw $t3, 12($sp)\n";
				ftext << "\tlw $t4, 16($sp)\n";
				ftext << "\tadd $sp, $sp, 16\n";
				ftext << "\tjr $ra\n";
			}
			break;
		}
		else if(gram=="StmtList"){
			// read Stmt
			cin >> n >> gram;
			if(gram=="Stmt") new_index=Stmt();
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
 	ftext << ".data\n";   
    for(auto i : vSymTable){
		if(i->func) continue;
        ftext << i->symbol << ":\t";

        if(i->type == "char"){
            if(i->arr)
                ftext << ".space\t" << (i->arr_size)*1 << endl;
            else
                ftext << ".byte\t\0\n";
        }
        else if(i->type == "int"){
            if(i->arr)
                ftext << ".space\t" << (i->arr_size)*4 << endl;
            else
                ftext << ".word\t0\n";
        }
        else if(i->type == "float"){
            if(i->arr)
                ftext << ".space\t" << (i->arr_size)*4 << endl;
            else
                ftext << ".float\t0.0\n";
        }
        else if(i->type == "double"){
            if(i->arr)
                ftext << ".space\t" << (i->arr_size)*8 << endl;
            else
                ftext << ".double\t0.0\n";
        }
    }
}

string SymbolTable::Stmt(string bkstmt){
	int n; string gram; cin >> n >> gram;
	if(gram==";") return "";
	else if(gram=="Expr"){
		Expr();
		cin >> n >> gram; //;
	}
	else if(gram=="return"){
		ftext << "\t# function return $v0\n";
		cin >> n >> gram; string id = Expr();
		cin >> n >> gram; //;
		if(id[0]=='$') ftext << "\tmove $v0, " << id << endl;
		else ftext << "\tlw $v0, " << id << endl;
	}
	else if(gram=="break"){
		cin >> n >> gram; //;
		ftext << "\tj " << bkstmt << endl;
	}
	else if(gram=="if"){
		cin >> n >> gram; // (
		cin >> n >> gram; string id = Expr();
		cin >> n >> gram; // )
		ftext << "\t# if stmt\n\t# compare zero\n";
		ftext << "\tlw $t1, " << id << endl;
		ftext << "\tbeq $t1, $zero, Else" << to_string(maxScope+1) << endl;
		cin >> n >> gram; Stmt(bkstmt);
		ftext << "\tj EndIf" << to_string(maxScope+1) << endl;
		cin >> n >> gram; // else
		ftext << "Else" << to_string(maxScope) << ":\n";
		cin >> n >> gram; Stmt(bkstmt);
		ftext << "EndIf" << to_string(maxScope) << ":\n";
	}
	else if(gram=="while"){
		cin >> n >> gram; // (
		cin >> n >> gram; string id = Expr();
		cin >> n >> gram; // )
		ftext << "\t# while loop\n\t# compare zero\n";
		ftext << "While"+to_string(maxScope+1) << ":\n";
		ftext << "\tlw $t1, " << id << endl;
		ftext << "\tbeq $t1, $zero, EndWhile" << to_string(maxScope+1) << endl;
		cin >> n >> gram; Stmt("EndWhile"+to_string(maxScope+1));
		ftext << "\tj While"+to_string(maxScope) << endl;
		ftext << "EndWhile" << to_string(maxScope) << ":\n";
	}
	else if(gram=="Block"){
		cin >> n >> gram; // {
		string new_index = "Block"+to_string(maxScope+1);
		newScope(new_index, false); 
	}
	else if(gram=="print"){
		cin >> n >> gram;
		cin >> n >> gram; string id = gram;
		cin >> n >> gram; //;
		ftext << "\t# print\n";
		ftext << "\tli $v0, 1\n";
		ftext << "\tlw $t1, " << id << endl;
		ftext << "\tadd $a0, $t1, $zero\n";
		ftext << "\tsyscall\n";
		ftext << "\tjr $ra\n";
	}
	return "";
}

string SymbolTable::Expr(){
	int n; string gram; cin >> n >> gram;
	if(gram=="UnaryOp"){
		cin >> n >> gram; string s = gram;
		cin >> n >> gram; string id = Expr();
		if(s=="-"){
			ftext << "\t# Unary minus\n";
			if(id[0]=='$') ftext << "\tmove $v0, " << id << endl;
			else ftext << "\tlw $v0, " << id << endl;
			ftext << "\tsub $t1, $zero, $t1\n";
			ftext << "\tsw $t1, " << id << endl;
		}
		else if(s=="!"){
			ftext << "\t# Not\n";
			if(id[0]=='$') ftext << "\tmove $v0, " << id << endl;
			else ftext << "\tlw $v0, " << id << endl;
			ftext << "\tnot $t1, $t1\n";
			ftext << "\tsw $t1, " << id << endl;
		}
		return id;
	}
	else if(gram=="num"){
		cin >> n >> gram; string num = gram;
		cin >> n >> gram; string id = Expr2(num,true);
        inorderExp.push(num);
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

string SymbolTable::Expr2(string pre, bool isNum){
	int n; string gram; cin >> n >> gram;
	if(gram=="BinOp"){
		cin >> n >> gram; string op = gram;
		cin >> n >> gram; string id = Expr();
        inorderExp.push(op);
		if(isNum){
			ftext << "\t# move num\n";
			if(pre[0]=='$') ftext << "\tmove $t4, " << pre << endl;
			else ftext << "\tlw $t4, " << pre << endl;
			pre="$t4";
		}
        else
            inorderExp.push(pre);
		if(op=="+"){
			ftext << "\t# Add\n";
			if(pre[0]=='$') ftext << "\tmove $t1, " << pre << endl;
			else ftext << "\tlw $t1, " << pre << endl;
			if(id[0]=='$') ftext << "\tmove $t2, " << id << endl;
			else ftext << "\tlw $t2, " << id << endl;
			ftext << "\tadd $t3, $t1, $t2\n";
		}
		else if(op=="-"){
			ftext << "\t# Sub\n";
			if(pre[0]=='$') ftext << "\tmove $t1, " << pre << endl;
			else ftext << "\tlw $t1, " << pre << endl;
			if(id[0]=='$') ftext << "\tmove $t2, " << id << endl;
			else ftext << "\tlw $t2, " << id << endl;
			ftext << "\tsub $t3, $t1, $t2\n"; 
		}
		else if(op=="*"){
			ftext << "\t# Mult\n";
			if(pre[0]=='$') ftext << "\tmove $t1, " << pre << endl;
			else ftext << "\tlw $t1, " << pre << endl;
			if(id[0]=='$') ftext << "\tmove $t2, " << id << endl;
			else ftext << "\tlw $t2, " << id << endl;
			ftext << "\tmul $t3, $t1, $t2\n";
		}
		else if(op=="/"){
			ftext << "\t# Div\n";
			if(pre[0]=='$') ftext << "\tmove $t1, " << pre << endl;
			else ftext << "\tlw $t1, " << pre << endl;
			if(id[0]=='$') ftext << "\tmove $t2, " << id << endl;
			else ftext << "\tlw $t2, " << id << endl;
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
		string id = Expr2(pre); 
		return id;
	}
	else if(gram=="("){
		cin >> n >> gram; 
		cin >> n >> gram; // ExprList
		cin >> n >> gram; // grammar in ExprList (ExprListTail or epsilon)
		if(gram=="ExprListTail") ExprListTail(0);
		cin >> n >> gram; // )
		ftext << "\tjal " << pre << endl;
		ftext << "\t# move function return to $t6\n";
		ftext << "\tmove $t6, $v0\n";
		string id = Expr2("$t6");
		return id;
	}
	else if(gram=="["){
		cin >> n >> gram; string id = Expr();
		cin >> n >> gram; // ]
		// get pre[id]
		ftext << "\t# move to array loc\n";
		ftext << "\tla $t5, " << pre << endl;
		ftext << "\tli $t4, " << id << endl;
		ftext << "\tadd $t4, $t4, $t4\n"; // double the index
		ftext << "\tadd $t4, $t4, $t4\n"; // index 4x
		if(symtable[pre].type=="double"){
			ftext << "\tadd $t4, $t4, $t4\n"; // index 6x
			ftext << "\tadd $t4, $t4, $t4\n"; // index 8x
		}
		ftext << "\tadd $t5, $t4, $t5\n";
		cin >> n >> gram; ExprArrayTail("$t5");
		return "$t5";
	}
	else if(gram=="="){
		cin >> n >> gram; string id = Expr();
        inorder2postorder();
        /*fstream ft;
        ft.open("exp.txt", ios::out);
        while(!postorderExp.empty()){
            ft << postorderExp.front() << " ";
            postorderExp.pop();
        }
        ft << endl;
        ft.close();*/
        //id = caculateExp();
		ftext << "\t# Equal\n";
		if(id[0]=='$') ftext << "\tmove $t1, " << id << endl;
		else ftext << "\tlw $t1, " << id << endl;
		ftext << "\tsw $t1, " << pre << endl;
	}
	return "epsilon";
}

void SymbolTable::inorder2postorder(){
    stack<string> temp;
    string item = inorderExp.top();
    inorderExp.pop();
    while(!inorderExp.empty()) {
        if(item == "(")
            temp.push(item);
        else if(item == ")"){
            while(!temp.empty() && (item = temp.top())!="("){
                postorderExp.push(item);
            }
            temp.pop();
        }
        else if(item == "+" || item == "-" || item == "*" || item == "/"){
            string item2 = inorderExp.top();
            while(priority(item2) > priority(item)){
                postorderExp.push(item2);
                temp.pop();
                item2 = temp.top();
            }
            temp.push(item);
        }
        else
            postorderExp.push(item);
        item = inorderExp.top();
        inorderExp.pop();
    }
    postorderExp.push(item);
    while(!temp.empty()){
        postorderExp.push(temp.top());
        temp.pop();
    }
}

int SymbolTable::priority(string item){
    if(item == "+" || item == "-")
        return 1;
    if(item == "*" || item == "/")
        return 2;
    return -1;
}

string SymbolTable::caculateExp(){
    stack<string> temp;
    string item = postorderExp.front();
    while(!postorderExp.empty()){
        while(item!="+" && item!="-" && item!="*" && item!="/"){
            ftext  << "item is " << item << endl;
            temp.push(item);
            ftext << "stack push " << item << endl;
            postorderExp.pop();
            item = postorderExp.front();
        }
        string pre = temp.top();
        ftext << "pre is "<<pre << endl;
        temp.pop();
        string id = temp.top();
        ftext << "id is" << id << endl;
        temp.pop();
        string result = getResult(pre, isNumber(pre), id, isNumber(id), item);
        temp.push(result);
        postorderExp.pop();
        item = postorderExp.front();
    }
    return temp.top()+"isResult";
}

string SymbolTable::getResult(string pre, bool preIsNum, string id, bool idIsNum, string op){
    if(preIsNum){
		ftext << "\t# move num\n";
		ftext << "\tli $t4, " << pre << endl;
		pre="$t4";
	}
    if(idIsNum){
		ftext << "\t# move num\n";
		ftext << "\tli $t5, " << id << endl;
		id="$t5";
    }

	if(op=="+"){
		ftext << "\t# Add\n";
		if(pre[0]=='$') ftext << "\tmove $t1, " << pre << endl;
		else ftext << "\tlw $t1, " << pre << endl;
		if(id[0]=='$') ftext << "\tmove $t2, " << id << endl;
		else ftext << "\tlw $t2, " << id << endl;
		ftext << "\tadd $t3, $t1, $t2\n";
	}
	else if(op=="-"){
		ftext << "\t# Sub\n";
		if(pre[0]=='$') ftext << "\tmove $t1, " << pre << endl;
		else ftext << "\tlw $t1, " << pre << endl;
		if(id[0]=='$') ftext << "\tmove $t2, " << id << endl;
		else ftext << "\tlw $t2, " << id << endl;
		ftext << "\tsub $t3, $t1, $t2\n"; 
	}
	else if(op=="*"){
		ftext << "\t# Mult\n";
		if(pre[0]=='$') ftext << "\tmove $t1, " << pre << endl;
		else ftext << "\tlw $t1, " << pre << endl;
		if(id[0]=='$') ftext << "\tmove $t2, " << id << endl;
		else ftext << "\tlw $t2, " << id << endl;
		ftext << "\tmul $t3, $t1, $t2\n";
	}
	else if(op=="/"){
		ftext << "\t# Div\n";
		if(pre[0]=='$') ftext << "\tmove $t1, " << pre << endl;
		else ftext << "\tlw $t1, " << pre << endl;
		if(id[0]=='$') ftext << "\tmove $t2, " << id << endl;
		else ftext << "\tlw $t2, " << id << endl;
		ftext << "\tdiv $t3, $t1, $t2\n";
	}
    else
        ftext << "op is " << op << endl; 
	return "$t3";
}

bool SymbolTable::isNumber(string item){
    for(int i=0; i<item.length(); i++)
        if(!isdigit(item[i]) || item[i]!='.')
            return false;
    return true;
}

void SymbolTable::ExprArrayTail(string pre){
	int n; string gram; cin >> n >> gram;
	if(gram=="Expr'"){
		Expr2(pre);
	}
	else if(gram=="="){
		cin >> n >> gram; string id = Expr();
		ftext << "\t# Equal\n";
		if(pre[0]=='$') ftext << "\tmove $t1, " << pre << endl;
		else ftext << "\tlw $t1, " << pre << endl;
		ftext << "\tsw $t1, " << pre << endl;
	}
}

void SymbolTable::ExprListTail(int i){
	int n; string gram; cin >> n >> gram; //Expr
	string id=Expr();
	ftext << "\t# move parameter to $ai\n";
	if(id[0]=='$') ftext << "\tmove $a, " << i << ", " << id << endl;
	else ftext << "\tlw $a, " << i << ", " << id << endl;
	cin >> n >> gram; //ExprListTail'
	cin >> n >> gram;
	if(gram==","){
		cin >> n >> gram;
		ExprListTail(i+1);
	}
}

