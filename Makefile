GXX = g++ -std=c++0x
#GXX = g++ -std=c++0x

all: Compiler.h hw3.cpp Set.o Lexer.o LLTable.o Tree.o SymbolTable.o
	$(GXX) hw3.cpp Set.o Lexer.o LLTable.o Tree.o SymbolTable.o -o hw3
#	g++ -Wall -g -o test hw3.cpp Set.o Lexer.o Tree.o -std=c++0x

Set.o: Set.h Set.cpp Compiler.h
	$(GXX) -c Set.cpp
#	g++ -std=c++0x -Wall -g -c Set.cpp

Lexer.o: Lexer.h Lexer.cpp
	$(GXX) -c Lexer.cpp
#	g++ -std=c++0x -Wall -g -c Lexer.cpp

LLTable.o: LLTable.h LLTable.cpp Set.h
	$(GXX) -c LLTable.cpp

Tree.o: Tree.h Tree.cpp Lexer.h LLTable.h
	$(GXX) -c Tree.cpp
#	g++ -std=c++0x -c -g Tree.cpp -Wall

SymbolTable.o: SymbolTable.h SymbolTable.cpp
	$(GXX) -c SymbolTable.cpp

clean:
	rm *.o hw3

