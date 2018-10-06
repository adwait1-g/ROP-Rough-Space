#include<bits/stdc++.h>
using namespace std;
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
    unsigned long int size, Imm;

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
            ptr = new movRegImmAST(str1, std::stoul(str2), size);
    }
    else if(tok1 == tok_mem_64) {

        if(tok2 == tok_reg_64)
            ptr = new movMemRegAST(str1, str2, size);
        
        else if(tok2 == tok_imm)
            ptr = new movMemImmAST(str1, std::stoul(str2), size);
    }

    //32-bit mov
    if(tok1 == tok_reg_32) {

        if(tok2 == tok_reg_32)
            ptr = new movRegRegAST(str1, str2, size);
        
        else if(tok2 == tok_mem_32) 
            ptr = new movRegMemAST(str1, str2, size);
        
        else if(tok2 == tok_imm)
            ptr = new movRegImmAST(str1, std::stoul(str2), size);
    }
    else if(tok1 == tok_mem_32) {

        if(tok2 == tok_reg_32)
            ptr = new movMemRegAST(str1, str2, size);
        
        else if(tok2 == tok_imm)
            ptr = new movMemImmAST(str1, std::stoul(str2), size);
    }

    //16-bit mov
    if(tok1 == tok_reg_16) {

        if(tok2 == tok_reg_16)
            ptr = new movRegRegAST(str1, str2, size);
        
        else if(tok2 == tok_mem_16) 
            ptr = new movRegMemAST(str1, str2, size);
        
        else if(tok2 == tok_imm)
            ptr = new movRegImmAST(str1, std::stoul(str2), size);
    }
    else if(tok1 == tok_mem_16) {

        if(tok2 == tok_reg_16)
            ptr = new movMemRegAST(str1, str2, size);
        
        else if(tok2 == tok_imm)
            ptr = new movMemImmAST(str1, std::stoul(str2), size);
    }

    //8-bit mov
    if(tok1 == tok_reg_8) {

        if(tok2 == tok_reg_8)
            ptr = new movRegRegAST(str1, str2, size);
        
        else if(tok2 == tok_mem_8) 
            ptr = new movRegMemAST(str1, str2, size);
        
        else if(tok2 == tok_imm)
            ptr = new movRegImmAST(str1, std::stoul(str2), size);
    }
    else if(tok1 == tok_mem_8) {

        if(tok2 == tok_reg_8)
            ptr = new movMemRegAST(str1, str2, size);
        
        else if(tok2 == tok_imm)
            ptr = new movMemImmAST(str1, std::stoul(str2), size);
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
        ptr = new pushImmAST(std::stoul(ImmValue));
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
            ptr = new addRegImmAST(str1, std::stoul(str2), size);
        }
    }

    else if(tok1 == tok_mem_64) {

        if(tok2 == tok_reg_64) {
            ptr = new addMemRegAST(str1, str2, size);
        }

        else if(tok2 == tok_imm) {
            ptr = new addMemImmAST(str1, std::stoul(str2), size);
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
            ptr = new addRegImmAST(str1, std::stoul(str2), size);
        }
    }

    else if(tok1 == tok_mem_32) {

        if(tok2 == tok_reg_32) {
            ptr = new addMemRegAST(str1, str2, size);
        }

        else if(tok2 == tok_imm) {
            ptr = new addMemImmAST(str1, std::stoul(str2), size);
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
            ptr = new addRegImmAST(str1, std::stoul(str2), size);
        }
    }

    else if(tok1 == tok_mem_16) {

        if(tok2 == tok_reg_16) {
            ptr = new addMemRegAST(str1, str2, size);
        }

        else if(tok2 == tok_imm) {
            ptr = new addMemImmAST(str1, std::stoul(str2), size);
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
            ptr = new addRegImmAST(str1, std::stoul(str2), size);
        }
    }

    else if(tok1 == tok_mem_8) {

        if(tok2 == tok_reg_8) {
            ptr = new addMemRegAST(str1, str2, size);
        }

        else if(tok2 == tok_imm) {
            ptr = new addMemImmAST(str1, std::stoul(str2), size);
        }
    }

    label->AddInst(ptr);
    getNextToken();
}
    

//Parsing sub instructions. 
void ParseSub(labelAST *label) {

    std::string str1, str2;
    int tok1, tok2;

    tok1 = getNextToken();
    str1 = IdentifierStr;

    tok2 = getNextToken();
    if(tok2 == tok_imm)
        str2 = ImmValue;
    else 
        str2 = IdentifierStr;
    
    cout<<"ParseSub: Operand1 = "<<str1<<", Operand2 = "<<str2<<endl;


    //64-bit operands. 
    if(tok1 == tok_reg_64) {

        if(tok2 == tok_reg_64) {
            ptr = new subRegRegAST(str1, str2, 64);
        }
        else if(tok2 == tok_mem_64) {
            ptr = new subRegMemAST(str1, str2, 64);
        }
        else if(tok2 == tok_imm) {
            ptr = new subRegImmAST(str1, std::stoul(str2), 64);
        }
    }

    else if(tok1 == tok_mem_64) {
        
        if(tok2 == tok_reg_64) {
            ptr = new subMemRegAST(str1, str2, 64);
        }
        else if(tok2 == tok_imm) {
            ptr = new subMemImmAST(str1, std::stoul(str2), 64);
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
            ptr = new subRegImmAST(str1, std::stoul(str2), 32);
        }
    }

    else if(tok1 == tok_mem_32) {
        
        if(tok2 == tok_reg_32) {
            ptr = new subMemRegAST(str1, str2, 32);
        }
        else if(tok2 == tok_imm) {
            ptr = new subMemImmAST(str1, std::stoul(str2), 32);
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
            ptr = new subRegImmAST(str1, std::stoul(str2), 16);
        }
    }

    else if(tok1 == tok_mem_16) {
        
        if(tok2 == tok_reg_16) {
            ptr = new subMemRegAST(str1, str2, 16);
        }
        else if(tok2 == tok_imm) {
            ptr = new subMemImmAST(str1, std::stoul(str2), 16);
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
            ptr = new subRegImmAST(str1, std::stoul(str2), 8);
        }
    }

    else if(tok1 == tok_mem_8) {
        
        if(tok2 == tok_reg_8) {
            ptr = new subMemRegAST(str1, str2, 8);
        }
        else if(tok2 == tok_imm) {
            ptr = new subMemImmAST(str1, std::stoul(str2), 8);
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

//Parsing and instruction. 
void ParseAnd(labelAST *label) {
    std::string str1, str2;
    int tok1, tok2;

    tok1 = getNextToken();
    str1 = IdentifierStr;

    tok2 = getNextToken();
    if(tok2 == tok_imm)
        str2 = ImmValue;

    else 
        str2 = IdentifierStr;
    
    //64-bit operands. 
    if(tok1 == tok_reg_64) {

        if(tok2 == tok_reg_64) 
            ptr = new andRegRegAST(str1, str2, 64);
        
        else if(tok2 == tok_mem_64) 
            ptr = new andRegMemAST(str1, str2, 64);
        
        else if(tok2 == tok_imm)
            ptr = new andRegImmAST(str1, std::stoul(str2), 64);
    }
    else if(tok1 == tok_mem_64) {

        if(tok2 == tok_reg_64) 
            ptr = new andMemRegAST(str1, str2, 64);
        
        else if(tok2 == tok_imm) 
            ptr = new andMemImmAST(str1, std::stoul(str2), 64);
    }

    //32-bti operands. 
    else if(tok1 == tok_reg_32) {

        if(tok2 == tok_reg_32)
            ptr = new andRegRegAST(str1, str2, 32);
        
        else if(tok2 == tok_mem_32)
            ptr = new andRegMemAST(str1, str2, 32);

        else if(tok2 == tok_imm)
            ptr = new andRegImmAST(str1, std::stoul(str2), 32);
    }
    else if(tok1 == tok_mem_32) {

        if(tok2 == tok_reg_32)
            ptr = new andMemRegAST(str1, str2, 32);
        
        else if(tok2 == tok_imm)
            ptr = new andMemImmAST(str1, std::stoul(str2), 32);
    }


    //16-bit operands. 
    else if(tok1 == tok_reg_16) {

        if(tok2 == tok_reg_16)
            ptr = new andRegRegAST(str1, str2, 16);
        
        else if(tok2 == tok_mem_16)
            ptr = new andRegMemAST(str1, str2, 16);
        
        else if(tok2 == tok_imm)
            ptr = new andRegImmAST(str1, std::stoul(str2), 16);
    }

    else if(tok1 == tok_mem_16) {

        if(tok2 == tok_reg_16) 
            ptr = new andMemRegAST(str1, str2, 16);
        
        else  if(tok2 == tok_imm)
            ptr = new andMemImmAST(str1, std::stoul(str2), 16);
    }

    //8-bit operands. 
    else if(tok1 == tok_reg_8) {

        if(tok2 == tok_reg_8)
            ptr = new andRegRegAST(str1, str2, 8);
        
        else if(tok2 == tok_mem_8)
            ptr = new andRegMemAST(str1, str2, 8);

        else if(tok2 == tok_imm)
            ptr = new andRegImmAST(str1, std::stoul(str2), 8);
    }
    else if(tok1 == tok_mem_8) {

        if(tok2 == tok_reg_8) 
            ptr = new andMemRegAST(str1, str2, 8);
        
        else if(tok2 == tok_imm)
            ptr = new andMemImmAST(str1, std::stoul(str2), 8);
    }

    label->AddInst(ptr);
    getNextToken();
}

//Parsing or instruction. 
void ParseOr(labelAST *label) {

    std::string str1, str2;
    int tok1, tok2;

    tok1 = getNextToken();
    str1 = IdentifierStr;

    tok2 = getNextToken();
    if(tok2 == tok_imm)
        str2 = ImmValue;
    else    
        str2 = IdentifierStr;
    
    //64-bit operands. 
    if(tok1 == tok_reg_64) {

        if(tok2 == tok_reg_64)
            ptr = new orRegRegAST(str1, str2, 64);
        
        else if(tok2 == tok_mem_64)
            ptr = new orRegMemAST(str1, str2, 64);
        
        else if(tok2 == tok_imm)
            ptr = new orRegImmAST(str1, std::stoul(str2), 64);
    }
    else if(tok1 == tok_mem_64) {

        if(tok2 == tok_reg_64) 
            ptr = new orMemRegAST(str1, str2, 64);
        
        if(tok2 == tok_imm)
            ptr = new orMemImmAST(str1, std::stoul(str2), 64);
    }
    
    //32-bit operands. 
    if(tok1 == tok_reg_32) {

        if(tok2 == tok_reg_32)
            ptr = new orRegRegAST(str1, str2, 32);
        
        else if(tok2 == tok_mem_32)
            ptr = new orRegMemAST(str1, str2, 32);
        
        else if(tok2 == tok_imm)
            ptr = new orRegImmAST(str1, std::stoul(str2), 32);
    }
    else if(tok1 == tok_mem_32) {

        if(tok2 == tok_reg_32) 
            ptr = new orMemRegAST(str1, str2, 32);
        
        if(tok2 == tok_imm)
            ptr = new orMemImmAST(str1, std::stoul(str2), 32);
    }

    //16-bit operands. 
    if(tok1 == tok_reg_16) {

        if(tok2 == tok_reg_16)
            ptr = new orRegRegAST(str1, str2, 16);
        
        else if(tok2 == tok_mem_16)
            ptr = new orRegMemAST(str1, str2, 16);
        
        else if(tok2 == tok_imm)
            ptr = new orRegImmAST(str1, std::stoul(str2), 16);
    }
    else if(tok1 == tok_mem_16) {

        if(tok2 == tok_reg_16) 
            ptr = new orMemRegAST(str1, str2, 16);
        
        if(tok2 == tok_imm)
            ptr = new orMemImmAST(str1, std::stoul(str2), 16);
    }

    //8-bit operands. 
    if(tok1 == tok_reg_8) {

        if(tok2 == tok_reg_8)
            ptr = new orRegRegAST(str1, str2, 8);
        
        else if(tok2 == tok_mem_8)
            ptr = new orRegMemAST(str1, str2, 8);
        
        else if(tok2 == tok_imm)
            ptr = new orRegImmAST(str1, std::stoul(str2), 8);
    }
    else if(tok1 == tok_mem_8) {

        if(tok2 == tok_reg_8) 
            ptr = new orMemRegAST(str1, str2, 8);
        
        if(tok2 == tok_imm)
            ptr = new orMemImmAST(str1, std::stoul(str2), 8);
    }

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

    else if(CurTok == tok_inst_and)
        ParseAnd(label);
    
    else if(CurTok == tok_inst_or)
        ParseOr(label);
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
