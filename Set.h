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
	private:
		void getFirst(struct eachTok &);
		void insertLLtable(string row,string col,string head);
};

#endif

