all: Compiler.h hw2.cpp Set.o Lexer.o LLTable.o Tree.o
	g++ -std=c++0x -o hw2 hw2.cpp Set.o Lexer.o LLTable.o Tree.o
#	g++ -Wall -g -o test hw2.cpp Set.o Lexer.o Tree.o -std=c++0x


Set.o: Set.h Set.cpp Compiler.h
	g++ -std=c++0x -c Set.cpp
#	g++ -std=c++0x -Wall -g -c Set.cpp

Lexer.o: Lexer.h Lexer.cpp Compiler.h
	g++ -std=c++0x -c Lexer.cpp
#	g++ -std=c++0x -Wall -g -c Lexer.cpp

LLTable.o: LLTable.h LLTable.cpp Compiler.h
	g++ -std=c++0x -c LLTable.cpp

Tree.o: Tree.h Tree.cpp Compiler.h
	g++ -std=c++0x -c Tree.cpp
#	g++ -std=c++0x -c -g Tree.cpp -Wall

clean:
	rm *.o hw2

