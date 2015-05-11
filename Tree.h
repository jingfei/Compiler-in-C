#ifndef TREE_H
#define TREE_H

#include "Compiler.h"

class Tree : Compiler{
	public:
		Tree(map <string, map<string, vector<string> > > LLtable,
				vector< pair<string,string> > Parse);
		void printTree();
	private:
		int Trace(string,vector< pair<string,string> >,int,int);
};

#endif
