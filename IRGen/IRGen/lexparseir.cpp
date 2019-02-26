#include "llvm/ADT/STLExtras.h"
#include<bits/stdc++.h>
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

static LLVMContext TheContext;
static IRBuilder<> Builder(TheContext);
static std::unique_ptr<Module> TheModule;
static std::map<std::string, Value *> NamedValues;





//=======================================LEXER START========================================//

//This is bad!
FILE *fp;


enum {
    tok_eof = -1, 

    tok_inst_mov = -2,
    tok_inst_push = -3,
    tok_inst_pop = -4,

    tok_inst_add = -5,
    tok_inst_sub = -6,
    tok_inst_inc = -7,
    tok_inst_dec = -8,
    tok_inst_mul = -9,
    tok_inst_div = -10,

    tok_reg_64 = -11,
    tok_reg_32 = -12,
    tok_reg_16 = -13,
    tok_reg_8 = -14,

    tok_mem_64 = -15,
    tok_mem_32 = -16,
    tok_mem_16 = -17,
    tok_mem_8 = -18,

    tok_imm = -19,

    tok_ret = -20,
    tok_call = -21,

    tok_label = -22,

    tok_unknown = -23,
};

std::string IdentifierStr; //For both RegisterNames, Instructions, Memory, Labels.
std::string ImmValue;   //For immediate values. 
unsigned long int OpSize;

int gettok(FILE *fp) {

    IdentifierStr.clear();
    ImmValue.clear();
    OpSize = 0;

    char LastChar = ' ';

    //Ignore all white spaces. 
    //Danger: The ',' parsing is not done properly. 
    while(isspace(LastChar) || LastChar == '\n' || LastChar == '\r')
        LastChar = fgetc(fp);
    
    //Simply eat up all comments. 
    if(LastChar == ';') {
        do 
            LastChar = fgetc(fp);
        while(LastChar != EOF && LastChar != '\n' && LastChar != '\r');

        if(LastChar != EOF)
            return gettok(fp);
    }
    
    //Identifiers - Instructions, Registers, Labels(Only alphabets(not even _))
    if(isalpha(LastChar)) {

        IdentifierStr = LastChar;
        while(isalnum(LastChar = fgetc(fp)))
            IdentifierStr += LastChar;
        
        //Memory manipulation Instructions. 
        if(IdentifierStr == "mov")
            return tok_inst_mov;
        
        else if(IdentifierStr == "pop")
            return tok_inst_pop;
        
        else if(IdentifierStr == "push")
            return tok_inst_push;
        
        //ALU Instructions. 
        else if(IdentifierStr == "add")
            return tok_inst_add;
        
        else if(IdentifierStr == "sub")
            return tok_inst_sub;
        
        else if(IdentifierStr == "mul")
            return tok_inst_mul;
        
        else if(IdentifierStr == "div")
            return tok_inst_div;

        else if(IdentifierStr == "inc")
            return tok_inst_inc;
        
        else if(IdentifierStr == "dec") 
            return tok_inst_dec;

        
        //ret
        else if(IdentifierStr == "ret")
            return tok_ret;
        
        //call
        else if(IdentifierStr == "call")
            return tok_call;
        
        //64-bit registers. 
        else if(IdentifierStr == "rax" || 
            IdentifierStr == "rbx" || 
            IdentifierStr == "rcx" || 
            IdentifierStr == "rdx" || 
            IdentifierStr == "rsi" || 
            IdentifierStr == "rdi" || 
            IdentifierStr == "rsp" || 
            IdentifierStr == "rbp" || 
            IdentifierStr == "r8" ||
            IdentifierStr == "r9" ||
            IdentifierStr == "r10" ||
            IdentifierStr == "r11" ||
            IdentifierStr == "r12" ||
            IdentifierStr == "r13" ||
            IdentifierStr == "r14" ||
            IdentifierStr == "r15") {
                OpSize = 64;
                return tok_reg_64;
            }
        
        //32-bit subregisters.
        else if(IdentifierStr == "eax" ||
            IdentifierStr == "ebx" || 
            IdentifierStr == "ecx" ||
            IdentifierStr == "edx" ||
            IdentifierStr == "esi" ||
            IdentifierStr == "edi" ||
            IdentifierStr == "r8d" || 
            IdentifierStr == "r9d" ||
            IdentifierStr == "r10d" ||
            IdentifierStr == "r11d" ||
            IdentifierStr == "r12d" ||
            IdentifierStr == "r13d" ||
            IdentifierStr == "r14d" ||
            IdentifierStr == "r15d") {
                OpSize = 32;
                return tok_reg_32;
            }
        
        //16-bit subregisters.
        else if(IdentifierStr == "ax" ||
            IdentifierStr == "bx" ||
            IdentifierStr == "cx" ||
            IdentifierStr == "dx" ||
            IdentifierStr == "si" ||
            IdentifierStr == "di" ||
            IdentifierStr == "r8w" ||
            IdentifierStr == "r9w" ||
            IdentifierStr == "r10w" ||
            IdentifierStr == "r11w" ||
            IdentifierStr == "r12w" ||
            IdentifierStr == "r13w" ||
            IdentifierStr == "r14w" ||
            IdentifierStr == "r15w") {
                OpSize = 16;
                return tok_reg_16;
            }
        
        //8-bit subregisters.
        else if(IdentifierStr == "al" ||
            IdentifierStr == "bl" ||
            IdentifierStr == "cl" ||
            IdentifierStr == "dl" ||
            IdentifierStr == "sil" ||
            IdentifierStr == "dil"||
            IdentifierStr == "r8b" ||
            IdentifierStr == "r9b" ||
            IdentifierStr == "r10b" ||
            IdentifierStr == "r11b" ||
            IdentifierStr == "r12b" ||
            IdentifierStr == "r13b" ||
            IdentifierStr == "r14b" ||
            IdentifierStr == "r15b") {
                OpSize = 8;
                return tok_reg_16;
            }
        
        //Label of the form LabelName:     (No Spaces between LabelName and  :)
        else if(LastChar == ':') {
            return tok_label;
        }
        
        else 
            cerr<<"Unknown Identifier: "<<IdentifierStr<<endl;
               return tok_unknown;
        
    }

    //Immediate values.
    if(isdigit(LastChar)) {

        std::string NumStr;
        char SecondChar;
        SecondChar = fgetc(fp);

        //Support for numbers of the form 0x....
        //For now, this lexer doesn't support hex numbers of the form 1234H or 1234h
        if(LastChar == '0' && SecondChar == 'x') {
            NumStr += LastChar;
            NumStr += SecondChar;

            while(isdigit(LastChar = fgetc(fp)))
                NumStr += LastChar;
        }
        
        //Support for normal decimal numbers. 
        else if(SecondChar != 'x') {
            NumStr += LastChar;
            NumStr += SecondChar;
            while(isdigit(LastChar = fgetc(fp)))
                NumStr += LastChar;
        }

        ImmValue = NumStr;
        return tok_imm;
    }   

    //Check for end of file
    else if(LastChar == EOF)
        return tok_eof;

    //Important (This is the ',' between 1st and 2nd operand)
    else if((LastChar = getchar()) == ',')
       return gettok(fp);
}

//===================================LEXER END===================================//

//===================================AST CLASSES=================================//

//=======================Memory Menipulation Instructions===================//

//Instruction base class
class instAST {
    public:
        virtual ~instAST() {}
        virtual Value *codegen() = 0;
};


//=========mov start!==========//
/*
//mov base class
class movAST {
    public:
        virtual ~movAST();
};
*/
//There are 5 categories in mov classfied based on the operands. 

//mov DstReg, SrcReg
class movRegRegAST : public instAST {
    std::string DstReg;
    std::string SrcReg;
    char size;

public:
    movRegRegAST(std::string &Reg1, std::string &Reg2, char s) 
        : DstReg(Reg1), SrcReg(Reg2), size(s) {}
    virtual Value *codegen();
};

//mov DstReg, SrcMem
class movRegMemAST : public instAST {
    std::string DstReg;
    std::string SrcMem;
    char size;

public:
    movRegMemAST(std::string &Reg, std::string &Mem, char s) 
        : DstReg(Reg), SrcMem(Mem), size(s) {}
    
    virtual Value *codegen();
};

//mov DstMem, SrcReg
class movMemRegAST : public instAST {
    std::string DstMem;
    std::string SrcReg;
    char size;

public:
    movMemRegAST(std::string &Mem, std::string &Reg, char s)
        : DstMem(Mem), SrcReg(Reg), size(s){}

    virtual Value *codegen();
};

//mov DstMem, SrcImm
class movMemImmAST : public instAST {
    std::string DstMem;
    std::string SrcImm;
    char size;

public:
    movMemImmAST(std::string &Mem, std::string &Imm, char s)
        : DstMem(Mem), SrcImm(Imm), size(s) {}

    virtual Value *codegen();

};

//mov DstReg, SrcImm
class movRegImmAST : public instAST {
    std::string DstReg;
    std::string SrcImm;
    char size;

public:
    movRegImmAST(std::string &Reg, std::string &Imm, char s)
        : DstReg(Reg), SrcImm(Imm), size(s) {}

        virtual Value *codegen();

};

//========mov done!=========//

//========pop start!========//

/*
//pop base class
class popAST {
    public:
        ~popAST() {}
};
*/
//There are 2 categories. 

//pop Reg
class popRegAST : public instAST {
    std::string DstReg;

public:
    popRegAST(std::string &Reg) : DstReg(Reg) {}

        virtual Value *codegen();

};

//pop Mem
class popMemAST : public instAST {
    std::string DstMem;

public:
    popMemAST(std::string &Mem) : DstMem(Mem) {}

        virtual Value *codegen();


};

//==========pop done!============//

//==========push start!==========//
/*
//push base class
class pushAST {
    public:
        ~pushAST() {}
};
*/
//There are 3 categories

//push Reg
class pushRegAST : public instAST {
    std::string SrcReg;

public:
    pushRegAST(std::string &Reg) : SrcReg(Reg) {}

        virtual Value *codegen();


};

//push Mem
class pushMemAST : public instAST {
    std::string SrcMem;

public:
    pushMemAST(std::string &Mem) : SrcMem(Mem) {}

        virtual Value *codegen();

};

//push Imm
class pushImmAST : public instAST {
    std::string SrcImm;

public:
    pushImmAST(std::string &Imm) : SrcImm(Imm) {}

        virtual Value *codegen();

};

//============push done!==============//

//==================ALU Instructions=================//

//==========add start!===========//
/*
//add base class
class addAST {
    public:
        ~addAST() {}
};
*/
//There are 5 categories

//add Reg, Reg
class addRegRegAST : public instAST {
    std::string DstReg;
    std::string SrcReg;
    char size;

public:
    addRegRegAST(std::string &Reg1, std::string &Reg2, char s) 
        : DstReg(Reg1), SrcReg(Reg2), size(s) {}

        virtual Value *codegen();

};

//add Reg, Mem
class addRegMemAST : public instAST {
    std::string DstReg;
    std::string SrcMem;
    char size;

public:
    addRegMemAST(std::string &Reg, std::string &Mem, char s) 
        : DstReg(Reg), SrcMem(Mem), size(s) {}

        virtual Value *codegen();

};

//add Reg, Imm
class addRegImmAST : public instAST {
    std::string DstReg;
    std::string SrcImm;
    char size;

public:
    addRegImmAST(std::string &Reg, std::string &Imm, char s) 
        : DstReg(Reg), SrcImm(Imm), size(s) {}

        virtual Value *codegen();

};

//add Mem, Reg
class addMemRegAST : public instAST {
    std::string DstMem;
    std::string SrcReg;
    char size;

public:
    addMemRegAST(std::string &Mem, std::string &Reg, char s) 
        : DstMem(Mem), SrcReg(Reg), size(s) {}

        virtual Value *codegen();
};

//add Mem, Imm
class addMemImmAST : public instAST {
    std::string DstMem;
    std::string SrcImm;
    char size;

public:
    addMemImmAST(std::string &Mem, std::string &Imm, char s) 
        : DstMem(Mem), SrcImm(Imm), size(s) {}

        virtual Value *codegen();


};

//==============add done!================//

//==============sub start!===============//
/*
//sub base class
class subAST {
    public:
        ~subAST() {}
};
*/
//There are 5 categories

//sub Reg, Reg
class subRegRegAST : public instAST {
    std::string DstReg;
    std::string SrcReg;
    char size;

public:
    subRegRegAST(std::string &Reg1, std::string &Reg2, char s) 
        : DstReg(Reg1), SrcReg(Reg2), size(s) {}

        virtual Value *codegen();


};

//sub Reg, Mem
class subRegMemAST : public instAST {
    std::string DstReg;
    std::string SrcMem;
    char size;

public:
    subRegMemAST(std::string &Reg, std::string &Mem, char s) 
        : DstReg(Reg), SrcMem(Mem), size(s) {}

        virtual Value *codegen();

};

//sub Reg, Imm
class subRegImmAST : public instAST {
    std::string DstReg;
    std::string SrcImm;
    char size;

public:
    subRegImmAST(std::string &Reg, std::string &Imm, char s) 
        : DstReg(Reg), SrcImm(Imm), size(s) {}

        virtual Value *codegen();

};

//sub Mem, Reg
class subMemRegAST : public instAST {
    std::string DstMem;
    std::string SrcReg;
    char size;

public:
    subMemRegAST(std::string &Mem, std::string &Reg, char s) 
        : DstMem(Mem), SrcReg(Reg), size(s) {}

        virtual Value *codegen();

};

//sub Mem, Imm
class subMemImmAST : public instAST {
    std::string DstMem;
    std::string SrcImm;
    char size;

public:
    subMemImmAST(std::string &Mem, std::string &Imm, char s) 
        : DstMem(Mem), SrcImm(Imm), size(s) {}

        virtual Value *codegen();

};

//==================sub done!================//

//==========inc start!================//
/*
//inc base class
class incAST {
    public:
        ~incAST() {}
};
*/
//There are 2 variations

//inc Reg
class incRegAST : public instAST {
    std::string Reg;
    char size;

public:
    incRegAST(std::string &R, char s) : Reg(R) , size(s){}

        virtual Value *codegen();

};

//inc Mem
class incMemAST : public instAST {
    std::string Mem;
    char size;

public:
    incMemAST(std::string &M, char s) : Mem(M) , size(s){}

        virtual Value *codegen();

};

//===========inc done!=============//

//===========dec start!===============//
/*
//dec base class
class decAST {
    public:
        ~decAST() {}
};
*/
//There are 2 variations.

//dec Reg
class decRegAST : public instAST {
    std::string Reg;
    char size;

public:
    decRegAST(std::string &R, char s) : Reg(R) , size(s){}

        virtual Value *codegen();

};

//dec Mem
class decMemAST : public instAST {
    std::string Mem;
    char size;

public:
    decMemAST(std::string &M, char s) : Mem(M) , size(s){}

        virtual Value *codegen();

};

//===============dec done!================//

//===============ret start!===============//
/*
//ret base class
class retAST {
    public:
        ~retAST() {}
};
*/
//There are 4 variations. 
//TODO: Learn what those variations mean. 

//=============ret done!================//

//==============call start!================//
/*
//call base class
class callAST {
    public:
        ~callAST() {}
};
*/
//There are 2 variations.

//call Reg
class callRegAST : public instAST {
    std::string Reg;

public:
    callRegAST(std::string &R) : Reg(R) {}

    virtual Value *codegen();

};

//call Imm
class callImmAST : public instAST {
    std::string Imm;

public:
    callImmAST(std::string &I) : Imm(I) {}

        virtual Value *codegen();

};

instAST *inst;

//===============call done!=====================//

//===============Label(Similar to a function)==================//


class labelAST {
    std::string LabelName;
    std::vector<instAST*> Inst;

public:
    labelAST(std::string Name){
        LabelName = Name;
    }
    
    void AddInst(instAST* I) {
        Inst.push_back(I);
    }

        virtual Value *codegen();

};
labelAST *label;

//=============Label done!=====================//

//=============root start!=======================//
class rootAST {
    std::vector<labelAST *> Label;

public:
    rootAST() {}

    void AddLabel(labelAST* L) {
        Label.push_back(L);
    }

        virtual Value *codegen();

};
static rootAST *Root;
//===============root done!=====================//

//==============================AST CLASSES DONE============================================//


//====================Parser Helper functions===========//


//Token Buffer
static int CurTok;
static int getNextToken() {
    CurTok = gettok(fp);
    return CurTok;
}

//3 Error handling routines. 
std::unique_ptr<instAST> LogError(const char *Str) {
    fprintf(stderr, "LogError: %s\n", Str);
    return nullptr;
}

std::unique_ptr<labelAST> LogErrorL(const char *Str) {
    LogError(Str);
    return nullptr;
}

std::unique_ptr<rootAST> LogErrorR(const char *Str) {
    LogError(Str);
    return nullptr;
}

instAST *ptr;

//Parsing mov instruction. 
void ParseMov(labelAST *label) {

    cout<<"Parsing mov"<<endl;

    int tok1, tok2;
    std::string str1, str2;
    unsigned long int size;

    tok1 = getNextToken();
    str1 = IdentifierStr;
    size = OpSize;

    cout<<"ParseMov: tok1 = "<<CurTok<<", str1 = "<<str1<<", size = "<<size<<endl; 

    tok2 = getNextToken();
    
    if(IdentifierStr.empty() && !ImmValue.empty() )
        str2 = ImmValue;
    
    else if(!IdentifierStr.empty() && ImmValue.empty())
        str2 = IdentifierStr;

    cout<<"ParseMov: tok2 = "<<CurTok<<", str2 = "<<str2<<",  size = "<<size<<endl; 
    

    //64-bit mov
    if(tok1 == tok_reg_64) {

        if(tok2 == tok_reg_64)
            ptr = new movRegRegAST(str1, str2, size);
        
        else if(tok2 == tok_mem_64) 
            ptr = new movRegMemAST(str1, str2, size);
        
        else if(tok2 == tok_imm)
            ptr = new movRegImmAST(str1, str2, size);
    }
    else if(tok1 == tok_mem_64) {

        if(tok2 == tok_reg_64)
            ptr = new movMemRegAST(str1, str2, size);
        
        else if(tok2 == tok_imm)
            ptr = new movMemImmAST(str1, str2, size);
    }

    //32-bit mov
    if(tok1 == tok_reg_32) {

        if(tok2 == tok_reg_32)
            ptr = new movRegRegAST(str1, str2, size);
        
        else if(tok2 == tok_mem_32) 
            ptr = new movRegMemAST(str1, str2, size);
        
        else if(tok2 == tok_imm)
            ptr = new movRegImmAST(str1, str2, size);
    }
    else if(tok1 == tok_mem_32) {

        if(tok2 == tok_reg_32)
            ptr = new movMemRegAST(str1, str2, size);
        
        else if(tok2 == tok_imm)
            ptr = new movMemImmAST(str1, str2, size);
    }

    //16-bit mov
    if(tok1 == tok_reg_16) {

        if(tok2 == tok_reg_16)
            ptr = new movRegRegAST(str1, str2, size);
        
        else if(tok2 == tok_mem_16) 
            ptr = new movRegMemAST(str1, str2, size);
        
        else if(tok2 == tok_imm)
            ptr = new movRegImmAST(str1, str2, size);
    }
    else if(tok1 == tok_mem_16) {

        if(tok2 == tok_reg_16)
            ptr = new movMemRegAST(str1, str2, size);
        
        else if(tok2 == tok_imm)
            ptr = new movMemImmAST(str1, str2, size);
    }

    //8-bit mov
    if(tok1 == tok_reg_8) {

        if(tok2 == tok_reg_8)
            ptr = new movRegRegAST(str1, str2, size);
        
        else if(tok2 == tok_mem_8) 
            ptr = new movRegMemAST(str1, str2, size);
        
        else if(tok2 == tok_imm)
            ptr = new movRegImmAST(str1, str2, size);
    }
    else if(tok1 == tok_mem_8) {

        if(tok2 == tok_reg_8)
            ptr = new movMemRegAST(str1, str2, size);
        
        else if(tok2 == tok_imm)
            ptr = new movMemImmAST(str1, str2, size);
    }

    label->AddInst(ptr);

    getNextToken();
}

//Parsing pop instruction. 
void ParsePop(labelAST *label) {

    getNextToken();

    cout<<"ParsePop: CurTok = "<<CurTok<<", IdentifierStr = "<<IdentifierStr<<endl;

    if(CurTok == tok_reg_64) {
        ptr = new popRegAST(IdentifierStr);
    }

    else if(CurTok == tok_mem_64) {
        ptr = new popMemAST(IdentifierStr);
    }

    label->AddInst(ptr);

    getNextToken();
}

//Parsing push instruction. 
void ParsePush(labelAST *label) {

    getNextToken();

    if(CurTok == tok_reg_64) {
        ptr = new pushRegAST(IdentifierStr);
    }

    else if(CurTok == tok_mem_64) {
        ptr = new pushMemAST(IdentifierStr);
    }

    else if(CurTok == tok_imm) {
        ptr = new pushImmAST(IdentifierStr);
    }
    
    cout<<"ParsePush: CurTok = "<<CurTok<<", "<<"IdentifierStr = "<<IdentifierStr<<", ImmValue = "<<ImmValue<<endl;

    label->AddInst(ptr);

    getNextToken();
}

//Parsing add instruction. 
void ParseAdd(labelAST *label) {

    std::string str1, str2;
    int tok1, tok2;
    int size;

    tok1 = getNextToken();
    str1 = IdentifierStr;
    size = OpSize;

    tok2 = getNextToken();
    if(tok2 == tok_imm) 
        str2 = ImmValue;
    
    else
        str2 = IdentifierStr;

    cout<<"ParseAdd: Operand1 = "<<str1<<", Operand2 = "<<str2<<endl;

    //64-bit operands. 
    if(tok1 == tok_reg_64) {

        if(tok2 == tok_reg_64) {
            ptr = new addRegRegAST(str1, str2, size);
        }

        else if(tok2 == tok_mem_64) {
            ptr = new addRegMemAST(str1, str2, size);
        }

        else if(tok2 == tok_imm) {
            ptr = new addRegImmAST(str1, str2, size);
        }
    }

    else if(tok1 == tok_mem_64) {

        if(tok2 == tok_reg_64) {
            ptr = new addMemRegAST(str1, str2, size);
        }

        else if(tok2 == tok_imm) {
            ptr = new addMemImmAST(str1, str2, size);
        }
    }


    //32-bit Operands. 
    if(tok1 == tok_reg_32) {

        if(tok2 == tok_reg_32) {
            ptr = new addRegRegAST(str1, str2, size);
        }

        else if(tok2 == tok_mem_32) {
            ptr = new addRegMemAST(str1, str2, size);
        }

        else if(tok2 == tok_imm) {
            ptr = new addRegImmAST(str1, str2, size);
        }
    }

    else if(tok1 == tok_mem_32) {

        if(tok2 == tok_reg_32) {
            ptr = new addMemRegAST(str1, str2, size);
        }

        else if(tok2 == tok_imm) {
            ptr = new addMemImmAST(str1, str2, size);
        }
    }

    //16-bit Operands
    if(tok1 == tok_reg_16) {

        if(tok2 == tok_reg_16) {
            ptr = new addRegRegAST(str1, str2, size);
        }

        else if(tok2 == tok_mem_16) {
            ptr = new addRegMemAST(str1, str2, size);
        }

        else if(tok2 == tok_imm) {
            ptr = new addRegImmAST(str1, str2, size);
        }
    }

    else if(tok1 == tok_mem_16) {

        if(tok2 == tok_reg_16) {
            ptr = new addMemRegAST(str1, str2, size);
        }

        else if(tok2 == tok_imm) {
            ptr = new addMemImmAST(str1, str2, size);
        }
    }

    //8-bit operands. 
    if(tok1 == tok_reg_8) {

        if(tok2 == tok_reg_8) {
            ptr = new addRegRegAST(str1, str2, size);
        }

        else if(tok2 == tok_mem_8) {
            ptr = new addRegMemAST(str1, str2, size);
        }

        else if(tok2 == tok_imm) {
            ptr = new addRegImmAST(str1, str2, size);
        }
    }

    else if(tok1 == tok_mem_8) {

        if(tok2 == tok_reg_8) {
            ptr = new addMemRegAST(str1, str2, size);
        }

        else if(tok2 == tok_imm) {
            ptr = new addMemImmAST(str1, str2, size);
        }
    }

    label->AddInst(ptr);
    getNextToken();
}
    

//Parsing sub instructions. 
void ParseSub(labelAST *label) {

    std::string str1, str2;
    int tok1, tok2, size;

    tok1 = getNextToken();
    str1 = IdentifierStr;
    size = OpSize;

    tok2 = getNextToken();
    if(tok2 == tok_imm)
        str2 = ImmValue;
    else 
        str2 = IdentifierStr;
    
    cout<<"ParseAdd: Operand1 = "<<str1<<", Operand2 = "<<str2<<endl;


    //64-bit operands. 
    if(tok1 == tok_reg_64) {

        if(tok2 == tok_reg_64) {
            ptr = new subRegRegAST(str1, str2, 64);
        }
        else if(tok2 == tok_mem_64) {
            ptr = new subRegMemAST(str1, str2, 64);
        }
        else if(tok2 == tok_imm) {
            ptr = new subRegImmAST(str1, str2, 64);
        }
    }

    else if(tok1 == tok_mem_64) {
        
        if(tok2 == tok_reg_64) {
            ptr = new subMemRegAST(str1, str2, 64);
        }
        else if(tok2 == tok_imm) {
            ptr = new subMemImmAST(str1, str2, 64);
        }
    }

    //32-bit operands. 
    if(tok1 == tok_reg_32) {

        if(tok2 == tok_reg_32) {
            ptr = new subRegRegAST(str1, str2, 32);
        }
        else if(tok2 == tok_mem_32) {
            ptr = new subRegMemAST(str1, str2, 32);
        }
        else if(tok2 == tok_imm) {
            ptr = new subRegImmAST(str1, str2, 32);
        }
    }

    else if(tok1 == tok_mem_32) {
        
        if(tok2 == tok_reg_32) {
            ptr = new subMemRegAST(str1, str2, 32);
        }
        else if(tok2 == tok_imm) {
            ptr = new subMemImmAST(str1, str2, 32);
        }
    }

    //16-bit operands. 
    if(tok1 == tok_reg_16) {

        if(tok2 == tok_reg_16) {
            ptr = new subRegRegAST(str1, str2, 16);
        }
        else if(tok2 == tok_mem_16) {
            ptr = new subRegMemAST(str1, str2, 16);
        }
        else if(tok2 == tok_imm) {
            ptr = new subRegImmAST(str1, str2, 16);
        }
    }

    else if(tok1 == tok_mem_16) {
        
        if(tok2 == tok_reg_16) {
            ptr = new subMemRegAST(str1, str2, 16);
        }
        else if(tok2 == tok_imm) {
            ptr = new subMemImmAST(str1, str2, 16);
        }
    }
    //8-bit operands. 
    if(tok1 == tok_reg_8) {

        if(tok2 == tok_reg_8) {
            ptr = new subRegRegAST(str1, str2, 8);
        }
        else if(tok2 == tok_mem_8) {
            ptr = new subRegMemAST(str1, str2, 8);
        }
        else if(tok2 == tok_imm) {
            ptr = new subRegImmAST(str1, str2, 8);
        }
    }

    else if(tok1 == tok_mem_8) {
        
        if(tok2 == tok_reg_8) {
            ptr = new subMemRegAST(str1, str2, 8);
        }
        else if(tok2 == tok_imm) {
            ptr = new subMemImmAST(str1, str2, 8);
        }
    }

    label->AddInst(ptr);
    getNextToken();
}


//Parsing inc instruction. 
void ParseInc(labelAST *label) {

    getNextToken();

    cout<<"ParseInc: IdentifierStr = "<<IdentifierStr<<endl;

    //64-bit operand. 
    if(CurTok == tok_reg_64)
        ptr = new incRegAST(IdentifierStr, 64);
    
    else if(CurTok == tok_mem_64)
        ptr = new incMemAST(IdentifierStr, 64);
    
    //32-bit operand. 
    else if(CurTok == tok_reg_32)
        ptr = new incRegAST(IdentifierStr, 32);
    
    else if(CurTok == tok_mem_32)
        ptr = new incMemAST(IdentifierStr, 32);
    
    //16-bit operand. 
    else if(CurTok == tok_reg_16)
        ptr = new incRegAST(IdentifierStr, 16);
    
    else if(CurTok == tok_mem_16)
        ptr = new incMemAST(IdentifierStr, 16);
    
    //8-bit operand. 
    else if(CurTok == tok_reg_8)
        ptr = new incRegAST(IdentifierStr, 8);
    
    else if(CurTok == tok_mem_8)
        ptr = new incMemAST(IdentifierStr, 8);
    
    label->AddInst(ptr);
    getNextToken();
}


//Parsing dec instruction. 
void ParseDec(labelAST *label) {

    getNextToken();

    cout<<"ParseDec: Operand = "<<IdentifierStr<<endl;

    //64-bit operand. 
    if(CurTok == tok_reg_64)
        ptr = new decRegAST(IdentifierStr, 64);
    
    else if(CurTok == tok_mem_64)
        ptr = new decRegAST(IdentifierStr, 64);
    
    //32-bit operand.
    else if(CurTok == tok_reg_32)
        ptr = new decRegAST(IdentifierStr, 32);
    
    else if(CurTok == tok_mem_32)
        ptr = new decMemAST(IdentifierStr, 32);
    
    //16-bit operand. 
    else if(CurTok == tok_reg_16)
        ptr = new decRegAST(IdentifierStr, 16);
    
    else if(CurTok == tok_mem_16)
        ptr = new decMemAST(IdentifierStr, 16);
    
    //8-bit operand. 
    else if(CurTok == tok_reg_8)
        ptr = new decRegAST(IdentifierStr, 8);
    
    else if(CurTok == tok_mem_8)
        ptr = new decMemAST(IdentifierStr, 8);

    label->AddInst(ptr);
    getNextToken();
}

/*
//Parsing ret instruction. 
//Support only for normal ret(c3) is provided for now. 
//TODO: Support for other 3 variations of ret. 
void ParseRet(labelAST *label) {

    getNextToken();
    cout<<"ParseRet: This is RET HAHAHAHA!"<<endl;
    ptr = new retAST(IdentifierStr);
    label->AddInst(ptr);

    getNextToken();
}
*/



//======================ParseAll: Takes care of all instructions in a label================//
int ParseAll(labelAST* label) {

    while(1) {
    cout<<"Inside ParseAll"<<endl;
    cout<<"ParseAll: CurTok = "<<CurTok<<", IdentifierStr = "<<IdentifierStr<<", ImmValue = "<<ImmValue<<endl;
    if(CurTok == tok_inst_mov) {
        ParseMov(label);
    }
  
    else if(CurTok == tok_inst_pop)
        ParsePop(label);
    
    else if(CurTok == tok_inst_push)
        ParsePush(label);

    else if(CurTok == tok_inst_add)
        ParseAdd(label);

    else if(CurTok == tok_inst_sub)
        ParseSub(label);
 /*   
    else if(CurTok == tok_inst_mul)
        //ParseMul(label);
    
    else if(CurTok == tok_inst_div)
        //ParseDiv(label);
*/
    
    else if(CurTok == tok_inst_inc)
        ParseInc(label);

    else if(CurTok == tok_inst_dec)
        ParseDec(label);
/*    
    else if(CurTok == tok_inst_ret)
        //ParseRet(label);
    
    else if(CurTok == tok_inst_call)
        //ParseCall(label);
 */   

    else if(CurTok == tok_eof)
        return tok_eof;

    else if(CurTok == tok_label)
        return tok_label;

    else if(CurTok == tok_unknown)
        cout<<"Unknown Identifier: "<<IdentifierStr<<endl;

    }

    getNextToken();
}

//=====================ParseLabel: Takes care of Labels======================//
int ParseLabel() {

    cout<<"Inside ParseLabel"<<endl;
    label = new labelAST(IdentifierStr);
    Root->AddLabel(label);

    getNextToken();

    while(ParseAll(label) != tok_eof && ParseAll(label) != tok_label);

    return 0;
}

//===========================HandleLabel: A Stub===========================//
static void HandleLabel() {

    cout<<"Inside HandleLabel"<<endl;
    if(ParseLabel() == 0)
        return;
    else
        cerr<<"Error while Parsing Label: "<<IdentifierStr<<endl;
}

//======================================Parser helper routines done!============================//

//=====================DisplayAST: Displays AST===========================//

void DisplayAST() {
    
}



//==============================MainLoop: Driver==========================/
static void MainLoop() {

    cout<<"In MainLoop\n"<<endl;

    Root = new rootAST;
    while(1) {
        switch(CurTok) {
            
            case tok_eof:
                cout<<"This is the AST I have built: \n\n"<<endl;
                DisplayAST();
                return;
            
            case tok_label:
                cout<<"Handling label"<<endl;
                HandleLabel();  
                break; 
        }                
    }
}


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