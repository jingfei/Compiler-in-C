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
			if(!ftext){cerr << "Can't open .text!\n"; exit(1);}
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
		void newScope(string,bool);
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
		};
		map <string,SymTable> symtable; 
		vector<SymTable*> vSymTable;
        stack<string> inorderExp;
        queue<string> postorderExp;
		/* for grammar */
		string Stmt(string bkstmt="");
		string Expr();
		string Expr2(string,bool isNum=false);
		string ExprIdTail(string);
		void ExprArrayTail(string);
		void ExprListTail(int);
        
        /* for expersion */
        void inorder2postorder();
        int priority(string);
        string caculateExp();
        string getResult(string, bool, string, bool, string);
        bool isNumber(string);
};

#endif

