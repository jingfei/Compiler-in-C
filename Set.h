#ifndef SET_H
#define SET_H

#include "Compiler.h"

class Set: public Compiler{
	public:
		void findFirst();
		void printFirst();
		void findFollow();
		void printFollow();
		void printLLtable();
		void Tree();
	private:
		void getFirst(struct eachTok &);
		void insertLLtable(string row,string col,string head);
		int Trace(string,vector< pair<string,string> >,int,int);
};

#endif

