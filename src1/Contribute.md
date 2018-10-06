## How to add support for new instruction?

**a.** Give a token code to the new instruction in Lexer.h .
**b.** Add code in the Lexer so that the Lexer is able to identify the new instruction. 

**c.** Add the relevant AST Classes in ASTClasses.h

**d.** Write a **ParseInst** function in Parser.h to take care of parsing the new instruction. 
**e.** Add code in **ParseAll** function so that it is able to identify the new instruction. 

Compile in the following manner and you are good to go!

**clang++ -g -O3 lexparseproper.cpp `llvm-config --cxxflags`**


