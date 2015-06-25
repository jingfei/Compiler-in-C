#Compiler hw3 

###author: E84016184 jingfei, H34011160 Wemy Ju
</br>
##main file

* hw3.cpp: all class's to finish inherit from Compiler.h, except for Symtable, it depends on tree.txt
</br>
##class files

* Compiler: define code struct and do input function according to grammar
* Lexer: inherit from Compiler, do parse from main.c
* Set: inherit from Compiler, find First and Follow set
* LLTable: inherit from Compiler, use First & Follow set to build LLTable
* Tree: inherit from Compiler, use LLTable to build main.c tree
* Symtable: read file from tree.txt, analysis grammar and generate MIPS code
</br>
## usage

* download `https://github.com/jingfei/Compiler-in-C.git`
* compile: `make`
* execute: `./hw3 input_.c_file`
