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

int paramNum,opNum,funcNum;

void SymbolTable::findSymbolTable(){
	opNum = funcNum = 0;
	ftext << ".text\n";
	newScope("0",false);
}

void SymbolTable::newScope(string index, bool moveStk, string bkstmt){ 
	int n; string gram,new_index="";
	stack< pair<int,string> > stk;
	scope.push(pair<int,string>(++maxScope,index)); 
	while(cin >> n >> gram){
		if(gram=="S" || gram=="Program" || gram=="$") continue;
		while(!stk.empty() && stk.top().first>=n) stk.pop();
		stk.push(pair<int,string>(n,gram));
		if(gram[0]=='{'){ 
			newScope(new_index, moveStk);
		}
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
			if(!stk.empty() && stk.top().second=="ParamDecl"){ 
				index = gram; // + to_string(maxScope+1); // param
				// gram = symbol
				ftext << "\t# move function parameter\n";
				if(isDouble(gram))
					ftext << "\ts.d $a" << (paramNum+=2) << ", " << gram << endl;
				else
					ftext << "\tsw $a" << paramNum++ << ", " << gram << endl;
			}
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
				paramNum=0;
				ftext << symtable[index].symbol << ":\n";
				moveStk=true;
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
				ftext << "\tjr $ra\n";
			}
			break;
		}
		else if(gram=="StmtList"){
			// read Stmt
			cin >> n >> gram;
			if(gram=="Stmt") new_index=(bkstmt=="" ? Stmt() : Stmt(bkstmt) );
		}
		if(new_index=="if" || new_index=="while"){
			new_index += to_string(maxScope+1);
		}
	}
	scope.pop();
}

void SymbolTable::printSymbolTable(){
	fstream fSymbol;
    fSymbol.open("symbol_table.txt", ios::out);
    for(auto i : vSymTable){
        if(i->symbol[0]!='$'){
		    fSymbol << setw(5)  << left << i->scope;
		    fSymbol << setw(10) << left << i->symbol;
		    fSymbol << setw(8)  << left << i->type;
		    fSymbol << setw(8)  << left << (i->arr ? "true" : "false");
	        fSymbol << setw(8)  << left << (i->func ? "true" : "false"); 
		    fSymbol << endl;
        }
	}
    fSymbol.close();
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
        while(!inorderExp.empty()) inorderExp.pop();
		cin >> n >> gram; string id = Expr();
		cin >> n >> gram; //;
        while(!postorderExp.empty()) postorderExp.pop();
		/*fstream ft1;
		  ft1.open("exp1.txt", ios::out);
		  while(!inorderExp.empty()){
		  ft1 << inorderExp.top() << " ";
		  inorderExp.pop();
		  }
		  ft1<< endl;*/
        inorder2postorder();
		/*fstream ft;
		  ft.open("exp2.txt", ios::out);
		  while(!postorderExp.empty()){
		  ft << postorderExp.front() << " ";
		  postorderExp.pop();
		  }
		  ft << endl;*/
        while(!inorderExp.empty()) inorderExp.pop();
        id = postorderExp.empty()?id:caculateExp(symtable[presentFun.top()].scope);
		if(isDouble(id)){
			if(id[0]=='$') ftext << "\tmov.d $v0, " << id << endl;
			else ftext << "\tl.d $v0, " << id << endl;
		}
		else{
			if(id[0]=='$') ftext << "\tmove $v0, " << id << endl;
			else ftext << "\tlw $v0, " << id << endl;
		}
		releaseRegister(id);
	}
	else if(gram=="break"){
		cin >> n >> gram; //;
		ftext << "\tj " << bkstmt << endl;
	}
	else if(gram=="if"){
		cin >> n >> gram; // (
		cin >> n >> gram; string id = Expr();
		cin >> n >> gram; // )
		ftext << "\t# if stmt\n";
		int ifScope = maxScope+1;
        while(!postorderExp.empty()) postorderExp.pop();
        inorder2postorder();
        while(!inorderExp.empty()) inorderExp.pop();
        id = postorderExp.empty()?id:caculateExp(ifScope);
		ftext << "\t# compare zero\n";
		string tmpReg; 
		if(isDouble(id)){
			tmpReg = chooseRegister(true);
			if(id[0]=='$') ftext << "\tmov.d " << tmpReg << ", " << id << endl;
			else ftext << "\tl.d " << tmpReg << ", " << id << endl;
			ftext << "\tc.eq.d " << tmpReg << ", $zero, Else" << ifScope << endl;
		}
		else{
			tmpReg = chooseRegister(false);
			if(id[0]=='$') ftext << "\tmove " << tmpReg << ", " << id << endl;
			else ftext << "\tlw " << tmpReg << ", " << id << endl;
			ftext << "\tbeq " << tmpReg << ", $zero, Else" << ifScope << endl;
		}
		releaseRegister(tmpReg);
		releaseRegister(id);
		cin >> n >> gram; Stmt(bkstmt);
		ftext << "\tj EndIf" << ifScope << endl;
		cin >> n >> gram; // else
		ftext << "Else" << ifScope << ":\n";
		cin >> n >> gram; Stmt(bkstmt);
		ftext << "EndIf" << ifScope << ":\n";
	}
	else if(gram=="while"){
		cin >> n >> gram; // (
		cin >> n >> gram; string id = Expr();
		cin >> n >> gram; // )
		ftext << "\t# while loop\n";
		int whileScope = maxScope+1;
		ftext << "While" << whileScope << ":\n";
        while(!postorderExp.empty()) postorderExp.pop();
        inorder2postorder();
        while(!inorderExp.empty()) inorderExp.pop();
        id = postorderExp.empty()?id:caculateExp(whileScope);
		ftext << "\t# compare zero\n";
		string tmpReg; 
		if(isDouble(id)){
			tmpReg = chooseRegister(true);
			if(id[0]=='$') ftext << "\tmov.d " << tmpReg << ", " << id << endl;
			else ftext << "\tl.d " << tmpReg << ", " << id << endl;
			ftext << "\tc.eq.d " << tmpReg << ", $zero, EndWhile" << whileScope << endl;
		}
		else{
			tmpReg = chooseRegister(false);
			if(id[0]=='$') ftext << "\tmove " << tmpReg << ", " << id << endl;
			else ftext << "\tlw " << tmpReg << ", " << id << endl;
			ftext << "\tbeq " << tmpReg << ", $zero, EndWhile" << whileScope << endl;
		}
		releaseRegister(tmpReg);
		releaseRegister(id);
		cin >> n >> gram; Stmt("EndWhile"+to_string(whileScope));
		ftext << "\tj While" << whileScope << endl;
		ftext << "EndWhile" << whileScope << ":\n";
	}
	else if(gram=="Block"){
		cin >> n >> gram; // {
		string new_index = "Block"+to_string(maxScope+1);
		newScope(new_index, false, bkstmt); 
	}
	else if(gram=="print"){
		cin >> n >> gram;
		cin >> n >> gram; string id = gram;
		cin >> n >> gram; //;
		ftext << "\t# print\n";
		if(isDouble(id)){
			ftext << "\tli $v0, 3\n";
			ftext << "\tl.d $f12, " << id << endl;
		}
		else{
			ftext << "\tli $v0, 1\n";
			ftext << "\tlw $a0, " << id << endl;
		}
		ftext << "\tsyscall\n";
	}
	return "";
}

string SymbolTable::Expr(){
	int n; string gram; cin >> n >> gram;
	if(gram=="UnaryOp"){
		cin >> n >> gram; string s = gram;
		cin >> n >> gram; string id = Expr();
        id = inorderExp.top();
        inorderExp.pop();
        if(isNumber(id)){
			string numReg;
			if(isDouble(id)){
				numReg = chooseRegister(true);
            	ftext << "\t# move num\n";
            	ftext << "\tli.d " << numReg << ", " << id << endl;
			}
			else{
				numReg = chooseRegister(false);
            	ftext << "\t# move num\n";
            	ftext << "\tli " << numReg << ", " << id << endl;
			}
            id = numReg;
        }
		string tmpReg = chooseRegister(isDouble(id));
		if(s=="-"){
			ftext << "\t# Unary minus\n";
			if(isDouble(id)){
				if(id[0]=='$') ftext << "\tmov.d " << tmpReg << ", " << id << endl;
				else ftext << "\tlw " << tmpReg << ", " << id << endl;
				ftext << "\tsub " << tmpReg << ", $zero, " << tmpReg << "\n";
				if(id[0]=='$') ftext << "\tmove " << id << ", " << tmpReg << "\n";
				else ftext << "\tsw " << tmpReg << ", " << id << endl;
			}
			else{
				if(id[0]=='$') ftext << "\tmove " << tmpReg << ", " << id << endl;
				else ftext << "\tlw " << tmpReg << ", " << id << endl;
				ftext << "\tsub " << tmpReg << ", $zero, " << tmpReg << "\n";
				if(id[0]=='$') ftext << "\tmove " << id << ", " << tmpReg << "\n";
				else ftext << "\tsw " << tmpReg << ", " << id << endl;
			}
		}
		else if(s=="!"){
			ftext << "\t# Not\n";
			if(isDouble(id)){
				if(id[0]=='$') ftext << "\tmov.d " << tmpReg << ", " << id << endl;
				else ftext << "\tl.d " << tmpReg << ", " << id << endl;
				ftext << "\tneg.d " << tmpReg << ", " << tmpReg << "\n";
				if(id[0]=='$') ftext << "\tmov.d " << id << ", " << tmpReg << "\n";
				else ftext << "\ts.d " << tmpReg << ", " << id << endl;
			}
			else{
				if(id[0]=='$') ftext << "\tmove " << tmpReg << ", " << id << endl;
				else ftext << "\tlw " << tmpReg << ", " << id << endl;
				ftext << "\tnot " << tmpReg << ", " << tmpReg << "\n";
				if(id[0]=='$') ftext << "\tmove " << id << ", " << tmpReg << "\n";
				else ftext << "\tsw " << tmpReg << ", " << id << endl;
			}
		}
		releaseRegister(id);
		symtable[tmpReg].isUsed=false;
        inorderExp.push(id);
		return "WemyGod!";
	}
	else if(gram=="num"){
		cin >> n >> gram; string num = gram;
		cin >> n >> gram; Expr2(num,true);
		inorderExp.push(num);
		return "WemyGod!";
	}
	else if(gram=="("){
        inStack.push(inorderExp); postStack.push(postorderExp);
        while(!inorderExp.empty()) inorderExp.pop();
        while(!postorderExp.empty()) postorderExp.pop();
		cin >> n >> gram; string id = Expr(); 
        inorder2postorder();
        id = caculateExp(maxScope);
		cin >> n >> gram; // )
        inorderExp = inStack.top();
        postorderExp = postStack.top();
        inStack.pop(); postStack.pop();
        cin >> n >> gram; Expr2(id);
		return "WemyGod!";
	}
	else if(gram=="id"){
		cin >> n >> gram; string id = gram;
		cin >> n >> gram; id = ExprIdTail(id);
		return id;
	}
	return "epsilon";
}

void SymbolTable::Expr2(string pre, bool isNum){
	int n; string gram; cin >> n >> gram;
	if(gram=="BinOp"){
		cin >> n >> gram; string op = gram;
		cin >> n >> gram; string id = Expr();
		inorderExp.push(op);
		if(!isNum) inorderExp.push(pre);
	}
	else if(gram=="epsilon"){
		if(!isNum) inorderExp.push(pre);
	}
}

string SymbolTable::ExprIdTail(string pre){
	int n; string gram; cin >> n >> gram;
	if(gram=="Expr'"){
		Expr2(pre);
	}
	else if(gram=="("){
        presentFun.push(pre);
		cin >> n >> gram; // ExprList
		cin >> n >> gram; // grammar in ExprList (ExprListTail or epsilon)
		if(gram=="ExprListTail") ExprListTail(0);
		cin >> n >> gram; // )
		ftext << "\tmove $s" << funcNum++ <<", $ra\n";
		symtable["$s"+to_string(funcNum-1)].isUsed=true;
		ftext << "\tjal " << pre << endl;
		ftext << "\tmove $ra, $s" << --funcNum << endl;
		symtable["$s"+to_string(funcNum)].isUsed=false;
		string funcReg;
		ftext << "\t# move function return to funcReg\n";
		if(isDouble(pre))
			ftext << "\tmov.d " << funcReg << ", $v0\n";
		else
			ftext << "\tmove " << funcReg << ", $v0\n";
        presentFun.pop();
		cin >> n >> gram; // Expr'
		Expr2(funcReg);
        while(!inorderExp.empty()) inorderExp.pop();
        inorderExp.push(funcReg);
	}
	else if(gram=="["){
		cin >> n >> gram; string id = Expr();
		cin >> n >> gram; // ]
		// get pre[id]
		while(!postorderExp.empty())
			postorderExp.pop();
		inorder2postorder();
		while(!inorderExp.empty())
			inorderExp.pop();
		id = postorderExp.empty() ? id : caculateExp(maxScope);
		ftext << "\t# move to array loc\n";
		string arrReg = chooseRegister(false);
		string arLocReg = chooseRegister(false);
		ftext << "\tla " << arrReg << ", " << pre << endl;
        symtable[arrReg].type = symtable[pre].type;
        symtable[arrReg].represent = pre;
		//id: num -> arLocReg
		if(id[0]=='$') ftext << "\tmove " << arLocReg << ", " << id << endl;
		else ftext << "\tlw " << arLocReg << ", " << id << endl;
		releaseRegister(id);
		ftext << "\tadd " << arLocReg << ", " << arLocReg << ", " << arLocReg << endl; // double the index
		ftext << "\tadd " << arLocReg << ", " << arLocReg << ", " << arLocReg << endl; // index 4x
		if(symtable[pre].type=="double"){
			ftext << "\tadd " << arLocReg << ", " << arLocReg << ", " << arLocReg << endl; // index 6x
			ftext << "\tadd " << arLocReg << ", " << arLocReg << ", " << arLocReg << endl; // index 8x
		}
		ftext << "\tadd " << arrReg << ", " << arLocReg << ", " << arrReg << endl;
		cin >> n >> gram; ExprArrayTail("0("+arrReg+")");
		symtable[arLocReg].isUsed = false;
		return arrReg;
	}
	else if(gram=="="){
		cin >> n >> gram; string id = Expr();
		while(!postorderExp.empty())
			postorderExp.pop();
		/*fstream ft1;
		  ft1.open("exp1.txt", ios::out);
		  while(!inorderExp.empty()){
		  ft1 << inorderExp.top() << " ";
		  inorderExp.pop();
		  }
		  ft1<< endl;*/
		inorder2postorder();
		while(!inorderExp.empty())
			inorderExp.pop();
		/*fstream ft;
		  ft.open("exp2.txt", ios::out);
		  while(!postorderExp.empty()){
		  ft << postorderExp.front() << " ";
		  postorderExp.pop();
		  }
		  ft << endl;*/
		id = postorderExp.empty() ? id : caculateExp(symtable[pre].scope);
        typeChecking(pre, symtable[id].represent, symtable[pre].scope);
		ftext << "\t# Equal\n";
		string tmpReg=chooseRegister(isDouble(id));
		if(isDouble(id)){
			if(id[0]=='$') ftext << "\tmov.d " << tmpReg << ", " << id << endl;
			else ftext << "\tl.d " << tmpReg << ", " << id << endl;
			if(pre[0]=='$') ftext << "\tmov.d " << pre << ", " << tmpReg << endl;
			else ftext << "\ts.d " << tmpReg << ", " << pre << endl;
		}
		else{
			if(id[0]=='$') ftext << "\tmove " << tmpReg << ", " << id << endl;
			else ftext << "\tlw " << tmpReg << ", " << id << endl;
			if(pre[0]=='$') ftext << "\tmove " << pre << ", " << tmpReg << endl;
			else ftext << "\tsw " << tmpReg << ", " << pre << endl;
		}
		releaseRegister(id);
	//	releaseRegister(pre);
		symtable[tmpReg].isUsed=false;
	}
	return pre;
}

void SymbolTable::inorder2postorder(){
	if(!inorderExp.empty()){
		stack<string> temp;
		string item;
		while(!inorderExp.empty()) {
			item = inorderExp.top();
			if(item == "(")
				temp.push(item);
			else if(item == ")"){
				while(!temp.empty()){
					if((item = temp.top())!="(")
						postorderExp.push(item);
					temp.pop();
				}
			}
			else if(item == "+" || item == "-" || item == "*" || item == "/" || item == "==" || item == "!=" || item == "<" || item == "<=" || item == ">" || item == ">=" || item == "&&" || item == "||"){
				string item2 = temp.empty() ? "-1" : temp.top();
				while(priority(item2) > priority(item)){
					postorderExp.push(item2);
					temp.pop();
					item2 = temp.empty() ? "-1" : temp.top();
				}
				temp.push(item);
			}
			else
				postorderExp.push(item);
			inorderExp.pop();
		}
		//postorderExp.push(item);
		while(!temp.empty()){
			postorderExp.push(temp.top());
			temp.pop();
		}
	}
}

int SymbolTable::priority(string item){
	if(item == "*" || item == "/") return 13;
	if(item == "+" || item == "-") return 12;
    if(item == "<" || item == "<=" || item == ">" || item == ">=") return 10;
    if(item == "==" || item == "!=") return 9;
    if(item == "&&") return 5;
    if(item == "||") return 4;
	return -1;
}

string SymbolTable::caculateExp(int scope){
    bool typeIsDouble;
	if(!postorderExp.empty()){
		if(postorderExp.size()<3)
            if(isNumber(postorderExp.front())){
				string numReg = chooseRegister(isDouble(postorderExp.front()));
		        ftext << "\t# move num\n";
				if(isDouble(postorderExp.front()))
		        	ftext << "\tli.d " << numReg << ", " << postorderExp.front() << endl;
				else
		        	ftext << "\tli " << numReg << ", " << postorderExp.front() << endl;
                symtable[numReg].type = isDouble(postorderExp.front())?"double":"int";
                symtable[numReg].turnType = isDouble(postorderExp.front())?true:false;
                symtable[numReg].represent = postorderExp.front();
                return numReg;
            }
            else
                return postorderExp.front(); 
		stack<string> temp;
        fstream ftp3;
        ftp3.open("exp3.txt", ios::out);
		string item = postorderExp.front();
        ftp3 << item <<" --out while\n";
		while(!postorderExp.empty()){
			while(item!="+" && item!="-" && item!="*" && item!="/" && item!="==" && item!="!=" && item!="<" && item!="<=" && item!=">" && item!=">=" && item!="&&" && item!="||"){
				temp.push(item);
				postorderExp.pop();
				item = postorderExp.front();
                ftp3 << item << " -- in while while\n";
			}
			string pre = temp.top(); temp.pop();
			string id = temp.top(); temp.pop();
            typeIsDouble = typeChecking(pre, id, scope);
			string result = getResult(pre, isNumber(pre), id, isNumber(id), item);
			temp.push(result);
            symtable[result].type = typeIsDouble ? "double" : "int";
			postorderExp.pop();
            if(!postorderExp.empty()) item = postorderExp.front();
            ftp3 << item << " -- in while\n";
		}
        symtable[temp.top()].represent = temp.top();
		return temp.top();
	}
}

string SymbolTable::getResult(string pre, bool preIsNum, string id, bool idIsNum, string op){
    if(preIsNum){
		string numReg=chooseRegister(isDouble(pre));
		ftext << "\t# move num\n";
		if(isDouble(pre))
			ftext << "\tli.d " << numReg << ", " << pre << endl;
		else
			ftext << "\tli " << numReg << ", " << pre << endl;
		pre=numReg;
        symtable[pre].type = isDouble(pre) ? "double" : "int";
        symtable[pre].turnType = isDouble(pre) ? true : false;
	}
	if(idIsNum){
		string idReg=chooseRegister(isDouble(id));
		ftext << "\t# move num\n";
		if(isDouble(id))
			ftext << "\tli.d " << idReg << ", " << id << endl;
		else
			ftext << "\tli " << idReg << ", " << id << endl;
		id=idReg;
        symtable[id].type = isDouble(id) ? "double" : "int";
        symtable[id].turnType = isDouble(id) ? true : false;
	}
	string opReg1, opReg2;
	if(isDouble(pre)){
		string opReg1 = chooseRegister(true);
		string opReg2 = chooseRegister(true);
		if(op=="+"){
			ftext << "\t# Add\n";
			if(pre[0]=='$') ftext << "\tmov.d " << opReg1 << ", " << pre << endl;
			else ftext << "\tl.d " << opReg1 << ", " << pre << endl;
			if(id[0]=='$') ftext << "\tmov.d " << opReg2 << ", " << id << endl;
			else ftext << "\tl.d " << opReg2 << ", " << id << endl;
			ftext << "\tadd.d " << opReg1 << ", " << opReg2 << ", " << opReg1 << endl;
		}
		else if(op=="-"){
			ftext << "\t# Sub\n";
			if(pre[0]=='$') ftext << "\tmov.d " << opReg1 << ", " << pre << endl;
			else ftext << "\tl.d " << opReg1 << ", " << pre << endl;
			if(id[0]=='$') ftext << "\tmov.d " << opReg2 << ", " << id << endl;
			else ftext << "\tl.d " << opReg2 << ", " << id << endl;
			ftext << "\tsub.d " << opReg1 << ", " << opReg2 << ", " << opReg1 << endl; 
		}
		else if(op=="*"){
			ftext << "\t# Mult\n";
			if(pre[0]=='$') ftext << "\tmov.d " << opReg1 << ", " << pre << endl;
			else ftext << "\tl.d " << opReg1 << ", " << pre << endl;
			if(id[0]=='$') ftext << "\tmov.d " << opReg2 << ", " << id << endl;
			else ftext << "\tl.d " << opReg2 << ", " << id << endl;
			ftext << "\tmul.d " << opReg1 << ", " << opReg2 << ", " << opReg1 << endl;
		}
		else if(op=="/"){
			ftext << "\t# Div\n";
			if(pre[0]=='$') ftext << "\tmov.d " << opReg1 << ", " << pre << endl;
			else ftext << "\tl.d " << opReg1 << ", " << pre << endl;
			if(id[0]=='$') ftext << "\tmov.d " << opReg2 << ", " << id << endl;
			else ftext << "\tl.d " << opReg2 << ", " << id << endl;
			ftext << "\tdiv.d " << opReg1 << ", " << opReg2 << ", " << opReg1 << endl;
		}
		else if(op=="&&"){
			ftext << "\t# op &&\n";
			if(pre[0]=='$') ftext << "\tmov.d " << opReg2 << ", " << pre << endl;
			else ftext << "\tl.d " << opReg2 << ", " << pre << endl;
			if(id[0]=='$') ftext << "\tmov.d " << opReg1 << ", " << id << endl;
			else ftext << "\tl.d " << opReg1 << ", " << id << endl;
			ftext << "\tc.eq.d " << opReg1 << ", $zero\n";
			ftext << "\tbc1t isFalse" << opNum << endl;
			ftext << "\tc.eq.d " << opReg2 << ", $zero\n";
			ftext << "\tbc1t isFalse" << opNum << endl;
			releaseRegister(opReg1);
			opReg1 = chooseRegister(false);
			ftext << "\tli " << opReg1 << ", 1\n";
			ftext << "\tj jTrue" << opNum << endl;
			ftext << "isFalse" << opNum << ":\n";
			ftext << "\tli " << opReg1 << ", 0\n";
			ftext << "jTrue" << opNum++ << ":\n";
		}
		else{
			ftext << "\t# op " << op << endl;
			if(pre[0]=='$') ftext << "\tmov.d " << opReg2 << ", " << pre << endl;
			else ftext << "\tl.d " << opReg2 << ", " << pre << endl;
			if(id[0]=='$') ftext << "\tmov.d " << opReg1 << ", " << id << endl;
			else ftext << "\tl.d " << opReg1 << ", " << id << endl;
			if(op=="||"){
				ftext << "\tc.eq.d " << opReg1 << ", $zero\n";
				ftext << "\tbc1f isTrue" << opNum << endl;
				ftext << "\tc.eq.d " << opReg2 << ", $zero\n";
				ftext << "\tbc1f isTrue" << opNum << endl;
			}
			else if(op=="!="){
				ftext << "\tc.eq.d " << opReg1 << ", " << opReg2 << endl;
				ftext << "\tbc1f isTrue" << opNum << endl;
			}
			else{
				if(op=="==") ftext << "\tc.eq.d " << opReg1 << ", " << opReg2 << endl;
				else if(op=="<") ftext << "\tc.lt.d " << opReg1 << ", " << opReg2 << endl;
				else if(op=="<=") ftext << "\tc.le.d " << opReg1 << ", " << opReg2 << endl;
				else if(op==">") ftext << "\tc.lt.d " << opReg2 << ", " << opReg1 << endl;
				else if(op==">=") ftext << "\tc.le.d " << opReg2 << ", " << opReg1 << endl;
				ftext << "\tbc1t isTrue" << opNum << endl;
			}
			releaseRegister(opReg1);
			opReg1 = chooseRegister(false);
			ftext << "\tli " << opReg1 << ", 0\n";
			ftext << "\tj jFalse" << opNum << endl;
			ftext << "isTrue" << opNum << ":\n";
			ftext << "\tli " << opReg1 << ", 1\n";
			ftext << "jFalse" << opNum++ << ":\n";
		}
	}
	else{
		opReg1 = chooseRegister(false);
		opReg2 = chooseRegister(false);
		if(op=="+"){
			ftext << "\t# Add\n";
			if(pre[0]=='$') ftext << "\tmove " << opReg1 << ", " << pre << endl;
			else ftext << "\tlw " << opReg1 << ", " << pre << endl;
			if(id[0]=='$') ftext << "\tmove " << opReg2 << ", " << id << endl;
			else ftext << "\tlw " << opReg2 << ", " << id << endl;
			ftext << "\tadd " << opReg1 << ", " << opReg2 << ", " << opReg1 << endl;
		}
		else if(op=="-"){
			ftext << "\t# Sub\n";
			if(pre[0]=='$') ftext << "\tmove " << opReg1 << ", " << pre << endl;
			else ftext << "\tlw " << opReg1 << ", " << pre << endl;
			if(id[0]=='$') ftext << "\tmove " << opReg2 << ", " << id << endl;
			else ftext << "\tlw " << opReg2 << ", " << id << endl;
			ftext << "\tsub " << opReg1 << ", " << opReg2 << ", " << opReg1 << endl; 
		}
		else if(op=="*"){
			ftext << "\t# Mult\n";
			if(pre[0]=='$') ftext << "\tmove " << opReg1 << ", " << pre << endl;
			else ftext << "\tlw " << opReg1 << ", " << pre << endl;
			if(id[0]=='$') ftext << "\tmove " << opReg2 << ", " << id << endl;
			else ftext << "\tlw " << opReg2 << ", " << id << endl;
			ftext << "\tmul " << opReg1 << ", " << opReg2 << ", " << opReg1 << endl;
		}
		else if(op=="/"){
			ftext << "\t# Div\n";
			if(pre[0]=='$') ftext << "\tmove " << opReg1 << ", " << pre << endl;
			else ftext << "\tlw " << opReg1 << ", " << pre << endl;
			if(id[0]=='$') ftext << "\tmove " << opReg2 << ", " << id << endl;
			else ftext << "\tlw " << opReg2 << ", " << id << endl;
			ftext << "\tdiv " << opReg1 << ", " << opReg2 << ", " << opReg1 << endl;
		}
		else if(op=="&&"){
			ftext << "\t# op &&\n";
			if(pre[0]=='$') ftext << "\tmove " << opReg2 << ", " << pre << endl;
			else ftext << "\tlw " << opReg2 << ", " << pre << endl;
			if(id[0]=='$') ftext << "\tmove " << opReg1 << ", " << id << endl;
			else ftext << "\tlw " << opReg1 << ", " << id << endl;
			ftext << "\tbeq " << opReg1 << ", $zero, isFalse" << opNum << endl;
			ftext << "\tbeq " << opReg2 << ", $zero, isFalse" << opNum << endl;
			ftext << "\tli " << opReg1 << ", 1\n";
			ftext << "\tj jTrue" << opNum << endl;
			ftext << "isFalse" << opNum << ":\n";
			ftext << "\tli " << opReg1 << ", 0\n";
			ftext << "jTrue" << opNum++ << ":\n";
		}
		else{
			ftext << "\t# op " << op << endl;
			if(pre[0]=='$') ftext << "\tmove " << opReg2 << ", " << pre << endl;
			else ftext << "\tlw " << opReg2 << ", " << pre << endl;
			if(id[0]=='$') ftext << "\tmove " << opReg1 << ", " << id << endl;
			else ftext << "\tlw " << opReg1 << ", " << id << endl;
			if(op=="==") ftext << "\tbeq " << opReg1 << ", " << opReg2 << ", isTrue" << opNum << endl;
			else if(op=="!=") ftext << "\tbne " << opReg1 << ", " << opReg2 << ", isTrue" << opNum << endl;
			else if(op=="<") ftext << "\tblt " << opReg1 << ", " << opReg2 << ", isTrue" << opNum << endl;
			else if(op=="<=") ftext << "\tble " << opReg1 << ", " << opReg2 << ", isTrue" << opNum << endl;
			else if(op==">") ftext << "\tbgt " << opReg1 << ", " << opReg2 << ", isTrue" << opNum << endl;
			else if(op==">=") ftext << "\tbge " << opReg1 << ", " << opReg2 << ", isTrue" << opNum << endl;
			else if(op=="||"){
				ftext << "\tbne " << opReg1 << ", $zero, isTrue" << opNum << endl;
				ftext << "\tbne " << opReg2 << ", $zero, isTrue" << opNum << endl;
			}
			ftext << "\tli " << opReg1 << ", 0\n";
			ftext << "\tj jFalse" << opNum << endl;
			ftext << "isTrue" << opNum << ":\n";
			ftext << "\tli " << opReg1 << ", 1\n";
			ftext << "jFalse" << opNum++ << ":\n";
		}
	}
	releaseRegister(pre);
	releaseRegister(id);
	releaseRegister(opReg2);
	return opReg1;
}

bool SymbolTable::isNumber(string item){
	if(item[0]>='0' && item[0]<='9' || item[0]=='.') return true;
	return false;
}

void SymbolTable::ExprArrayTail(string pre){
	int n; string gram; cin >> n >> gram;
	if(gram=="Expr'"){
		Expr2(pre);
	}
	else if(gram=="="){
		cin >> n >> gram; string id = Expr();
		while(!postorderExp.empty()) postorderExp.pop();
		/*fstream ft1;
		  ft1.open("exp1.txt", ios::out);
		  while(!inorderExp.empty()){
		  ft1 << inorderExp.top() << " ";
		  inorderExp.pop();
		  }
		  ft1<< endl;*/
		inorder2postorder();
		/*fstream ft;
		  ft.open("exp2.txt", ios::out);
		  while(!postorderExp.empty()){
		  ft << postorderExp.front() << " ";
		  postorderExp.pop();
		  }
		  ft << endl;*/
		while(!inorderExp.empty()) inorderExp.pop();
		string tempPre = pre.substr(2,3);
        id = caculateExp(symtable[tempPre].scope);
        typeChecking(symtable[tempPre].represent, symtable[id].represent, symtable[symtable[tempPre].represent].scope);
        ftext << "\t# Equal\n";
		string tmpReg=chooseRegister(isDouble(id));
		if(isDouble(id)){
			if(id[0]=='$') ftext << "\tmov.d " << tmpReg << ", " << id << endl;
			else ftext << "\tl.d " << tmpReg << ", " << id << endl;
			if(pre[0]=='$') ftext << "\tmov.d " << pre << ", " << tmpReg << endl;
			else ftext << "\ts.d " << tmpReg << ", " << pre << endl;
		}
		else{
			if(id[0]=='$') ftext << "\tmove " << tmpReg << ", " << id << endl;
			else ftext << "\tlw " << tmpReg << ", " << id << endl;
			if(pre[0]=='$') ftext << "\tmove " << pre << ", " << tmpReg << endl;
			else ftext << "\tsw " << tmpReg << ", " << pre << endl;
		}
		releaseRegister(pre);
		releaseRegister(id);
		symtable[tmpReg].isUsed=false;
	}
}

void SymbolTable::ExprListTail(int i){
	int n; string gram; cin >> n >> gram; //Expr
	string id=Expr();
	ftext << "\t# move parameter to $ai\n";
	if(isDouble(id)){
		ftext << "\tl.d $a" << i << ", " << id << endl;
		symtable["$a"+to_string(i)].type="double";
		symtable["$a"+to_string(i)].turnType=true;
	}
	else
		ftext << "\tlw $a" << i << ", " << id << endl;
	releaseRegister(id);
	cin >> n >> gram; //ExprListTail'
	cin >> n >> gram;
	if(gram==","){
		cin >> n >> gram;
		if(isDouble(id)) ExprListTail(i+2);
		else ExprListTail(i+1);
	}
}

bool SymbolTable::typeChecking(string a, string b, int scope){
    string typeA, typeB, idA = a, idB=b;
    bool aIsNum=false, bIsNum=false;
    if(isNumber(a)){
        if(isDouble(a)) typeA = "double";
        else typeA = "int";
        aIsNum = true;
    }
    else if(a[0] == '$'){
        idA = "temp";
        typeA = symtable[a].turnType ? "double" : symtable[a].type;
    }
    else typeA = symtable[a].turnType ? "double" : symtable[a].type;
    if(isNumber(b)){
        if(isDouble(b)) typeB = "double";
        else typeB = "int";
        bIsNum = true;
    }
    else if(b[0] == '$'){
        idB = "temp";
        typeB = symtable[b].turnType ? "double" : symtable[b].type;
    }
    else typeB= symtable[b].turnType ? "double" : symtable[b].type;
    if(typeA!=typeB){
        cout << "warning (scope " << scope << ") : " << idA << " " <<  typeA << " , " << idB << " " << typeB << endl;
        if(!aIsNum) symtable[a].turnType = true;
        if(!bIsNum) symtable[b].turnType = true;
        return true;
    }
    return false;
}

bool SymbolTable::isDouble(string item){
	if(isNumber(item)){
	    for(int i=1; i<item.length(); ++i)
	        if(item[i] == '.')
	            return true;
		return false;
	}
	else if(item[0]=='$'){
		if(item[1]=='f') return true;
		return symtable[item].turnType;
	}
	return symtable[item].turnType;
}

string SymbolTable::chooseRegister(bool isdouble){
	if(isdouble){
		for(int i=0; i<32; i+=2)
			if(!symtable["$f"+to_string(i)].isUsed){
				symtable["$f"+to_string(i)].isUsed=true;
				symtable["$f"+to_string(i)].turnType=true;
				return "$f"+to_string(i);
			}
		return "WemyDoubleReg!";
	}
    for(int i=0; i<10; ++i)
        if(!symtable["$t"+to_string(i)].isUsed){
			symtable["$t"+to_string(i)].isUsed=true;
            symtable["$t"+to_string(i)].turnType=false;
            return "$t"+to_string(i);
		}
    for(int i=7; i>=0; --i)
        if(!symtable["$s"+to_string(i)].isUsed){
			symtable["$s"+to_string(i)].isUsed=true;
            symtable["$t"+to_string(i)].turnType=false;
            return "$s"+to_string(i);
		}
    return "WemyReg!"; // may cause problem
}

void SymbolTable::releaseRegister(string t){
	string tmp="$";
	for(int i=0; i<t.length(); ++i)
		if(t[i]=='$' && (t[i+1]=='t' || t[i+1]=='s' || t[i+1]=='f') ){
			tmp+=t[i+1];
			tmp+=t[i+2];
			break;
		}
	if(tmp.length()>1) symtable[tmp].isUsed=false;
}

