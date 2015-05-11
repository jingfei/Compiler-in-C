#ifndef LLTABLE_H
#define LLTABLE_H

#include "Compiler.h"

class LLTable: public Compiler{
	public:
		LLTable(map <string,Grammar> Gram,	
					map <string,eachTok> Toks,
					vector <string> Seq);
		void findLLtable();
		void printLLtable();
		map <string, map<string, vector<string> > > getLLtable();
	private:
		void insertLLtable(string row,string col,string head);
};

#endif
