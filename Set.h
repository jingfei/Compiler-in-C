#ifndef SET_H
#define SET_H

#include "Compiler.h"

class Set: public Compiler{
	public:
//		Set(map <string,Grammar> Gram, 
//			map <string,eachTok> Toks, 
//			vector <string> Seq);
		void findFirst();
		void printFirst(fstream&);
		void findFollow();
		void printFollow(fstream&);
		map <string,eachTok> getToks();
	private:
		void getFirst(struct eachTok &);
};

#endif

