#ifndef TREE_H
#define TREE_H

#include "Lexer.h"
#include "LLTable.h"

class Tree : public Lexer, public LLTable{
	public:
//		Tree(map <string, map<string, vector<string> > > LLtable,
//				vector< pair<string,string> > Parse);
		void printTree(fstream&);
	private:
		int Trace(string,vector< pair<string,string> >,int,int,fstream&);
};

#endif
