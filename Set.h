#ifndef SET_H
#define SET_H

#include "Compiler.h"

class Set: public Compiler{
	public:
		void findFirst();
		void printFirst();
		void findFollow();
	private:
		void getFirst(struct eachTok &);
};

#endif

