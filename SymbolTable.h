#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <stack>
#include <iostream>
#include <fstream>
using namespace std;

class SymbolTable{
	public:
		SymbolTable(){
			maxScope=-1; 
			ftext.open(".text", ios::out);
			if(!ftext){cerr << "Can't open .text!\n"; exit(1);}
		}
		void findSymbolTable();
		void printSymbolTable();
		void output();
	private:
		fstream ftext;
		/* for scope */
		int maxScope;
		stack< pair<int,string> > scope;
		void newScope(string,bool);
		/* for output */
		struct var{
			var() : res(""), arg1(""), arg2("") {}
//			bool isMain;
//			int scope;
//			vector< pair<int,string> > tree;
			string res;
			string arg1,arg2;
		};
//		vector<func*> vfuncs;
//		map<string,func> mfuncs;
		struct SymTable{
			/* for table */
			int scope;
			string symbol;
			string type;
			bool arr;
			bool func;
			/* for stmt */
			double value;
			vector<double> ar;
			int func_scope;
			bool isMain;
		};
		map <string,SymTable> symtable; // index is id+scope_num
		vector<SymTable*> vSymTable;
		/* for grammar */
//		void StmtList(vector< pair<int,string> >::iterator it);
		void Stmt();
		string Expr();
		string Expr2(string);
		string ExprIdTail(string);
};

#endif

