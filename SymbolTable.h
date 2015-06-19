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
        void genDotDataFile();
	private:
		fstream ftext;
		/* for scope */
		int maxScope;
		stack< pair<int,string> > scope;
		void newScope(string);
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
			string func_name;
		};
		map <string,SymTable> symtable; // index is id+scope_num
		vector<SymTable*> vSymTable;
		/* for grammar */
		void Stmt();
		string Expr();
		string Expr2(string);
		string ExprIdTail(string);
};

#endif

