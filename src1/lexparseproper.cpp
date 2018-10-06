#include "llvm/ADT/STLExtras.h"
#include<bits/stdc++.h>
#include"Lexer.h"
#include"ASTClasses.h"
#include"Parser.h"
using namespace std;


//1. This is an attempt to write a complete lexer and parser for ROP Gadgets. 
//2. Support is given to 2 instructions - mov and pop. 
//3. After understanding the concept, will extend it to all the common instructions. 


//          IDEAL CONTROL FLOW OF THE PARSER: (Lexer helps the parser!) - getNextToken() - sexy job!
//                                              int main()
//                                                  |
//                                              MainLoop()            
//                                                  |    
//               --------------if eof---------------|--------if new label found----------
//               |                                                                      |
//           DisplayAST()                                                            HandleLabel() - Just a Stub!
//               |                                                                      |   
//           IR Generation using AST(Next job!)                                      ParseLabel() - Adds label to root's vector                    
//                                                                                      |           
//                                                                            -->----ParseAll() - Has options to parse all instructions. 
//                                                                            |         |    
//                                                                            |         ----mov------ParseMov()
//                                                                            |         |
//                                                                            |         ---push------ParsePush()
//                                                                            |         |
//                                                                            |         ----pop------ParsePop()
//                                                                            |         |
//                                                                            |         ----add------ParseAdd()
//                                                                            |         |
//                                                                            |         ----sub------ParseSub()
//                                                                            |         |
//                                                                            |         ----inc------ParseInc()
//                                                                            |         |
//                                                                            |         ----dec------ParseDec()
//                                                                            |         |    
//                                                                            ---<------|    1. The lexer works properly. No bugs encoutered so far. 
//2. Handle the above mentioned asm instructions - Operands being registers and 
//
//  
//
//
//Improvements required immediately: 
//
//1. Support for Memory - operands. - Eg: DWORD PTR[rbp-0x10] - Work on Lexer
//2. Memory locations which are immediate values : Work on parser. (Lexer will give it as an immediate value)
//3. Error handling - Not written so far assuming we get proper instructions because we will be parsing already made executables. 
//4. Adding support for more instructions 







//=====================PROGRAM START======================//

int main(int argc, char **argv) {

    if(argc != 2) {
        fprintf(stderr, "Usage: $ %s <assemblyfile_name>\n", argv[0]);
        return -1;
    }

    //FILE *fp;
    fp = fopen(argv[1], "r");
    if(fp == NULL) {
        cerr<<"Error in opening the assembly file"<<endl;
        return -1;
    }

    getNextToken();
    MainLoop();

    fclose(fp);
    return 0;
}