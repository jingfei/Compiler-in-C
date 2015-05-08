#ifndef LEXER_H
#define LEXER_H

#include "Compiler.h"

class Lexer: public Compiler{
	public:
		Lexer( );
		vector< pair<string,string> > getParse();
	private:
		vector<string> line;
		string analyze(string, char);
		void Parser(char,string);
		bool isOper(string);
		bool isKey(string);
		bool isSpec(char);
		bool isComment(string);
		bool isIden(string);
		bool isNum(string);
		bool isChar(string);
};

#endif

