#include<bits/stdc++.h>
using namespace std;
//=======================================LEXER START========================================//

//This is bad!
FILE *fp;


enum {
    tok_eof = -1, 

    tok_inst_mov = -2,
    tok_inst_push = -3,
    tok_inst_pop = -4,

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

    tok_inst_add = -5,
    tok_inst_sub = -6,
    tok_inst_inc = -7,
    tok_inst_dec = -8,
    tok_inst_mul = -9,
    tok_inst_div = -10,
    tok_inst_or = -24,
    tok_inst_and = -25,
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
        
        else if(IdentifierStr == "or")
            return tok_inst_or;
        
        else if(IdentifierStr == "and")
            return tok_inst_and;

        
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

