#ifndef LLTABLE_H
#define LLTABLE_H

#include "Set.h"

class LLTable: public Set{
	public:
//		LLTable(map <string,Grammar> Gram,	
//					map <string,eachTok> Toks,
//					vector <string> Seq);
		void findLLtable();
		void printLLtable();
		map <string, map<string, vector<string> > > getLLtable();
	private:
		void insertLLtable(string row,string col,string head);
};

#endif
