#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <iostream>
using namespace std;

class Lexer{
	public:
//		Lexer(vector< pair<string,string> > Parse);
		void findLexer();
		vector< pair<string,string> > Parse;
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

