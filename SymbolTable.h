#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <stack>
#include <queue>
#include <iostream>
#include <fstream>
using namespace std;

class SymbolTable{
	public:
		SymbolTable(){
			maxScope=-1; 
			ftext.open("main.asm", ios::out);
			if(!ftext){cerr << "Can't open main.asm!\n"; exit(1);}
            presentFun.push("main");
		}
		void findSymbolTable();
		void printSymbolTable();
        void genDotDataFile();
        ~SymbolTable(){
            ftext.close();
        }
	private:
		fstream ftext;
		/* for scope */
		int maxScope;
		stack< pair<int,string> > scope;
		void newScope(string,bool,string bkstmt="");
		struct SymTable{
			/* for table */
			int scope;
			string symbol;
			string type;
			bool arr;
			int arr_size;
			bool func;
			/* for stmt */
			double value;
			vector<double> ar;
			int func_scope;
			string func_name;
            bool turnType;
            bool isUsed;
            string represent;
//			string tmpReplace;
			/* constructor */
			SymTable(): /*tmpReplace(""),*/turnType(false),isUsed(false),arr(false),func(false){}
		};
		map <string,SymTable> symtable; 
		vector<SymTable*> vSymTable;
        stack<string>inorderExp;
        queue<string>postorderExp;
        stack<stack<string> >inStack;
        stack<queue<string> >postStack;
        stack<string> presentFun;
        /* for grammar */
		string Stmt(string bkstmt="");
		string Expr();
		void Expr2(string,bool isNum=false);
		string ExprIdTail(string);
		void ExprArrayTail(string);
		void ExprListTail(int);
        /* for expersion */
        void inorder2postorder();
        int priority(string);
        string caculateExp(int scope = 1000);
        string getResult(string, bool, string, bool, string);
        bool isNumber(string);
        /* for type checking*/
        bool typeChecking(string &, string &, int, bool isEqual);
        bool isDouble(string);
		/* for temp */
        string chooseRegister(bool isdouble);
		void releaseRegister(string);
		void returnType();
};

#endif

