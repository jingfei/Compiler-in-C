#include <iostream>
#include <cstdio>
#include <cctype>
#include <vector>
#include <iomanip>
#include "Lexer.h"
#include "Set.h"
using namespace std;

void Lexer::Parser(char tok, string val){
	pair<string,string> nextParse;
	if(tok=='i'){ //identifier
		nextParse.first = "id";
		nextParse.second=val;
	}
	else if(tok=='c'){  //char
		nextParse.first = "char";
		nextParse.second=val;
	}
	else if(tok=='n'){  //numbver
		nextParse.first = "num";
		nextParse.second=val;
	}
	else{
		nextParse.first = val;
		nextParse.second= "";
	}
	Parse.push_back(nextParse);
}

void Lexer::findLexer(){
	string s,tmp;
	Parse.push_back(pair<string,string>("S",""));
	while(getline(cin,s)){
		int Len = s.length();
		tmp="";
		for(int i=0; i<Len; ++i){
			tmp = analyze(tmp,s[i]);
			if(isComment(tmp)) break;
		}
		if(!isComment(tmp)) analyze(tmp, ' ');
	}
	Parse.push_back(pair<string,string>("$",""));
}

vector< pair<string,string> > Lexer::getParse(){
	return Parse;
}

string Lexer::analyze(string s, char cNew){
	int Len = s.length();
	string cString = ""; cString+=cNew;
	if(isSpec(cNew)){
		analyze(s,' ');
		string tmp=""; tmp+=cNew;
		Parser('s',tmp);
		return "";
	}
	else if(Len==0) return (cNew==' ' || cNew=='\t') ? "" : cString;
	else if(Len==1 && (isOper(s) || s=="&" || s=="|") ){
		if(isComment(s+cString)) return "//";
		if(isOper(s+cString)){
			Parser('o',s+cString);
			return "";
		}
		else{
			Parser('o', s);
			return (cNew==' ' || cNew=='\t') ? "" : cString;
		}
	}
	else if(isOper(cString) || cNew=='&' || cNew=='|'){
		analyze(s, ' ');
		return cString;
	}
	else if(s[0]=='\'' && cNew!='\'') return s+cString;
	else if(s[0]=='\''){
		Parser('c', s+cString);
		return "";
	}
	else if(cNew==' ' || cNew=='\t'){
		if(isKey(s)) Parser('k', s);
		else if(isOper(s)) Parser('o', s);
		else if(isNum(s)) Parser('n', s);
		else if(isChar(s)) Parser('c', s);
		else if(isIden(s)) Parser('i', s);
		else Parser('e', s);
		return "";
	}
	return s+cString;
}

bool Lexer::isOper(string s){
	if(s=="=" || s=="!" || s=="+" || s=="-" || s=="*" || s=="/" || s=="!=" 
		|| s=="<" || s==">" || s=="<=" || s==">=" || s=="&&" || s=="||" 
		|| s=="==") 
		return true;
	return false;
}

bool Lexer::isKey(string s){
	if(s=="int" || s=="char" || s=="float" || s=="double" || s=="return" ||
		s=="if" || s=="else" || s=="while" || s=="break" || s=="for" ||
		s=="print")
		return true;
	return false;
}

bool Lexer::isSpec(char c){
	if(c=='[' || c==']' || c=='(' || c==')' || c=='{' || c=='}' || c==';' ||
		c==',') return true;
	return false;
}

bool Lexer::isComment(string s){
	if(s=="//") return true;
	return false;
}

bool Lexer::isIden(string s){
	if(!isalpha(s[0]) && s[0]!='_') return false;
	return true;
}

bool Lexer::isNum(string s){
	int Len = s.length();
	bool hasDot = false;
	for(int i=0; i<Len; ++i){
		if(isdigit(s[i])) continue;
		if(s[i]=='.' && !hasDot) hasDot=true;
		else return false;
	}
	if(s[0]=='.' || s[Len-1]=='.') return false;
	if(Len==1 && hasDot) return false;
	return true;
}

bool Lexer::isChar(string s){
	int Len = s.length();
	if(s[0]=='\'' && s[Len-1]=='\'') return true;
	return false;
}

