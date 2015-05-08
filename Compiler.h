#ifndef COMPILER_H
#define COMPILER_H

#include <cstdio>
#include <map>
#include <set>
#include <vector>
#include <cstring>
#include <iostream>
using namespace std;

class Compiler{
	struct Grammar{
		string name;
		vector < vector<string> > content;
	};
	
	struct eachTok{
		string name;
		bool isTerm;
		set <string> First;
		set <string> Follow;
	};
	protected:
		vector <string> Seq;
		map <string,Grammar> Gram;
		map <string,eachTok> Toks;
		map <string, map<string, vector<string> > > LLtable;
	public:
		vector< pair<string,string> > Parse;
		void input(){
			/* first */
			struct Grammar tmp;
			tmp.name="S";
			vector <string> inFirst;
			inFirst.push_back("Program");
			inFirst.push_back("$");
			tmp.content.push_back(inFirst);
			Gram[tmp.name]=tmp;
			struct eachTok tmpTok;
			tmpTok.name=tmp.name;
			tmpTok.isTerm=true;
			Toks[tmp.name]=tmpTok;
			/* others */
			char line[10000];
			while(fgets(line,sizeof(line),stdin)!=0){
				if(line[0]=='\t'){
					vector <string> inTmp;
					char *unit=strtok(line,"\n\t ");
					while(unit!=NULL){
						inTmp.push_back(unit);
						/* find Toks */
						if(Toks.find(unit)==Toks.end()){
							tmpTok.name=unit;
							tmpTok.isTerm=true;
							Toks[unit]=tmpTok;
						}
						/*************/
						unit=strtok(NULL,"\n\t ");	
					}
					tmp.content.push_back(inTmp);
				}
				else{
					Seq.push_back(tmp.name);
					Gram[tmp.name]=tmp;
					tmp.name=strtok(line,"\n");
					tmp.content.clear();
					/*find Toks*/
					if(Toks.find(tmp.name)==Toks.end()){
						tmpTok.name = tmp.name;
						tmpTok.isTerm=false;
						Toks[tmp.name]=tmpTok;
					}
					else{
						Toks.find(tmp.name)->second.isTerm=false;
					}
				}
			}
			Seq.push_back(tmp.name);
			Gram[tmp.name]=tmp;
			Toks["S"].isTerm=false;
		}
};

#endif

