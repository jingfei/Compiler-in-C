#Compiler hw2 (by E84016184 jingfei)

##main file

* hw2.cpp: use Compiler class to finish

##class files

* Compiler: define code struct and do input function according to grammar
* Lexer: inherit from Compiler, do parse from main.c
* Set: inherit from Compiler, find First and Follow set
* LLTable: inherit from Compiler, use First & Follow set to build LLTable
* Tree: inherit from Compiler, use LLTable to build main.c tree
