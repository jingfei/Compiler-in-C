all: Set.o hw2.cpp
	g++ -std=c++0x -o hw2 hw2.cpp Set.o


Set.o: Set.h Set.cpp Compiler.h
	g++ -std=c++0x -c Set.cpp

