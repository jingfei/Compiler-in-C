#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <stack>
#include <iostream>
using namespace std;

class SymbolTable{
	public:
		SymbolTable(){
			maxScope=-1; 
		}
		void findSymbolTable();
		void printSymbolTable();
		void output();
        void genDotDataFile();
	private:
		/* for scope */
		int maxScope;
		stack< pair<int,string> > scope;
		void newScope(string,bool);
		/* for output */
		struct func{
			bool isMain;
			int scope;
			vector< pair<int,string> > tree;
		};
		vector<func*> vfuncs;
		map<string,func> mfuncs;
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
		void StmtList(vector< pair<int,string> >::iterator it);
};

#endif

