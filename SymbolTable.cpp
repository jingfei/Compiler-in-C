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
        if(gram[0]=='{'){ 
            //			ftext << "Symbol" << (moveStk ? "true" : "false") << endl;
            if(new_index[0]=='i' && new_index[1]=='f')
                newScope(new_index, false); 
            else if(new_index[0]=='w' && new_index[1]=='h' && new_index[2]=='i'
                    && new_index[3]=='l' && new_index[4]=='e')
                newScope(new_index, false);
            else
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
                //				ftext << "func" << (moveStk ? "true" : "false") << endl;
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
            //			ftext << (moveStk ? "true" : "false") << endl;
            // read Stmt
            cin >> n >> gram;
            if(gram=="Stmt") new_index=Stmt();
        }
        if(new_index=="if" || new_index=="while"){
            new_index += to_string(maxScope+1);
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
                fp << ".word\t0\n";
        }
        else if(i->type == "float"){
            if(i->arr)
                fp << ".space\t" << (i->arr_size)*4 << endl;
            else
                fp << ".float\t0.0\n";
        }
        else if(i->type == "double"){
            if(i->arr)
                fp << ".space\t" << (i->arr_size)*8 << endl;
            else
                fp << ".double\t0.0\n";
        }
    }
    fp.close();
}

string SymbolTable::Stmt(){
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
        ftext << "\tlw $v0, " << id << endl;
    }
    //	else if(gram=="break"){
    //		cin >> n >> gram; //;
    //	}
    else if(gram=="if"){
        return "if";
    }
    else if(gram=="while"){
        return "while";
    }
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
    return "";
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
        inExpr2 = true; 
        inorderExp.push(op);
        if(isNum){
       //     ftext << "\t# move num\n";
       //     ftext << "\tli $t4, " << pre << endl;
            pre="$t4";
        }
        else
            inorderExp.push(pre);
       /* if(op=="+"){
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
        }*/
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
        ftext << "\tlw $t6, $v0\n";
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
        cin >> n >> gram; ExprArrayTail("0($t5)");
        return "0($t5)";
    }
    else if(gram=="="){
        cin >> n >> gram; string id = Expr();
        while(!postorderExp.empty())
            postorderExp.pop();
        inorder2postorder();
        while(!inorderExp.empty())
            inorderExp.pop();
        if(inExpr2){
   /* fstream ft;
    ft.open("exp.txt", ios::out);
            while(!postorderExp.empty()){
                ft << postorderExp.front() << " ";
                postorderExp.pop();
            }
            ft << endl;*/
            id = caculateExp();
        }
        ftext << "\t# Equal\n";
        ftext << "\tlw $t1, " << id << endl;
        ftext << "\tsw $t1, " << pre << endl;
    }
    return "epsilon";
}

void SymbolTable::inorder2postorder(){
    if(inExpr2){
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
            temp.push(item);
            postorderExp.pop();
            item = postorderExp.front();
        }
        string pre = temp.top();
        temp.pop();
        string id = temp.top();
        temp.pop();
        string result = getResult(pre, isNumber(pre), id, isNumber(id), item);
        temp.push(result);
        postorderExp.pop();
        item = postorderExp.front();
    }
    inExpr2 = false;
    return temp.top();
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
        ftext << "\tlw $t1, " << id << endl;
        ftext << "\tsw $t1, " << pre << endl;
    }
}

void SymbolTable::ExprListTail(int i){
    int n; string gram; cin >> n >> gram; //Expr
    string id=Expr();
    ftext << "\t# move parameter to $ai\n";
    ftext << "\tlw $a" << i << ", " << id << endl;
    cin >> n >> gram; //ExprListTail'
    cin >> n >> gram;
    if(gram==","){
        cin >> n >> gram;
        ExprListTail(i+1);
    }
}

