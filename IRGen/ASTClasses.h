#include<bits/stdc++.h>


//===================================AST CLASSES=================================//

//=======================Memory Menipulation Instructions===================//

//Instruction base class
class instAST {
    public:
        virtual ~instAST() {}
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
    
    
};


//mov DstReg, SrcMem
class movRegMemAST : public instAST {
    std::string DstReg;
    std::string SrcMem;
    char size;

public:
    movRegMemAST(std::string &Reg, std::string &Mem, char s) 
        : DstReg(Reg), SrcMem(Mem), size(s) {}

   
};

//mov DstMem, SrcReg
class movMemRegAST : public instAST {
    std::string DstMem;
    std::string SrcReg;
    char size;

public:
    movMemRegAST(std::string &Mem, std::string &Reg, char s)
        : DstMem(Mem), SrcReg(Reg), size(s){}
    
     
};

//mov DstMem, SrcImm
class movMemImmAST : public instAST {
    std::string DstMem;
    unsigned long SrcImm;
    char size;

public:
    movMemImmAST(std::string &Mem, unsigned long Imm, char s)
        : DstMem(Mem), SrcImm(Imm), size(s) {}
    
     
};

//mov DstReg, SrcImm
class movRegImmAST : public instAST {
    std::string DstReg;
    unsigned long SrcImm;
    char size;

public:
    movRegImmAST(std::string &Reg, unsigned long Imm, char s)
        : DstReg(Reg), SrcImm(Imm), size(s) {}
    
    
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

    
};

//pop Mem
class popMemAST : public instAST {
    std::string DstMem;

public:
    popMemAST(std::string &Mem) : DstMem(Mem) {}

   
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

    
};

//push Mem
class pushMemAST : public instAST {
    std::string SrcMem;

public:
    pushMemAST(std::string &Mem) : SrcMem(Mem) {}

   
};

//push Imm
class pushImmAST : public instAST {
    unsigned long SrcImm;

public:
    pushImmAST(unsigned long Imm) : SrcImm(Imm) {}

    
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
    
   
};

//add Reg, Mem
class addRegMemAST : public instAST {
    std::string DstReg;
    std::string SrcMem;
    char size;

public:
    addRegMemAST(std::string &Reg, std::string &Mem, char s) 
        : DstReg(Reg), SrcMem(Mem), size(s) {}
    
    
};

//add Reg, Imm
class addRegImmAST : public instAST {
    std::string DstReg;
    unsigned long int SrcImm;
    char size;

public:
    addRegImmAST(std::string &Reg, unsigned long Imm, char s) 
        : DstReg(Reg), SrcImm(Imm), size(s) {}
    
    
};

//add Mem, Reg
class addMemRegAST : public instAST {
    std::string DstMem;
    std::string SrcReg;
    char size;

public:
    addMemRegAST(std::string &Mem, std::string &Reg, char s) 
        : DstMem(Mem), SrcReg(Reg), size(s) {}
    
    
};

//add Mem, Imm
class addMemImmAST : public instAST {
    std::string DstMem;
    unsigned long int SrcImm;
    char size;

public:
    addMemImmAST(std::string &Mem, unsigned long Imm, char s) 
        : DstMem(Mem), SrcImm(Imm), size(s) {}
    
   
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
    
    
};

//sub Reg, Mem
class subRegMemAST : public instAST {
    std::string DstReg;
    std::string SrcMem;
    char size;

public:
    subRegMemAST(std::string &Reg, std::string &Mem, char s) 
        : DstReg(Reg), SrcMem(Mem), size(s) {}
    
    
};

//sub Reg, Imm
class subRegImmAST : public instAST {
    std::string DstReg;
    unsigned long int SrcImm;
    char size;

public:
    subRegImmAST(std::string &Reg, unsigned long int Imm, char s) 
        : DstReg(Reg), SrcImm(Imm), size(s) {}
    
    
};

//sub Mem, Reg
class subMemRegAST : public instAST {
    std::string DstMem;
    std::string SrcReg;
    char size;

public:
    subMemRegAST(std::string &Mem, std::string &Reg, char s) 
        : DstMem(Mem), SrcReg(Reg), size(s) {}
    
    
};

//sub Mem, Imm
class subMemImmAST : public instAST {
    std::string DstMem;
    unsigned long int SrcImm;
    char size;

public:
    subMemImmAST(std::string &Mem, unsigned long int Imm, char s) 
        : DstMem(Mem), SrcImm(Imm), size(s) {}
    
    
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

    
};

//inc Mem
class incMemAST : public instAST {
    std::string Mem;
    char size;

public:
    incMemAST(std::string &M, char s) : Mem(M) , size(s){}

    
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

    
};

//dec Mem
class decMemAST : public instAST {
    std::string Mem;
    char size;

public:
    decMemAST(std::string &M, char s) : Mem(M) , size(s){}

    
};

//===============dec done!================//

//===============and start!===============//

//There are 5 variations.  - like mov


//and Reg, Reg
class andRegRegAST : public instAST {
    std::string SrcReg;
    std::string DstReg;
    unsigned char size;

public:
    andRegRegAST(std::string &Reg1, std::string &Reg2, unsigned char s) 
        : SrcReg(Reg1), DstReg(Reg2), size(s) {}
};


//and Reg, Mem
class andRegMemAST : public instAST {
    std::string SrcMem;
    std::string DstReg;
    unsigned char size;

public:
    andRegMemAST(std::string &Reg, std::string &Mem, unsigned char s)
        : DstReg(Reg), SrcMem(Mem), size(s) {}
};

//and Reg, Imm
class andRegImmAST : public instAST {
    std::string DstReg;
    unsigned long SrcImm;
    unsigned char size;

public:
    andRegImmAST(std::string &Reg, unsigned long Imm, unsigned char s) 
        : DstReg(Reg), SrcImm(Imm), size(s) {}
};

//and Mem, Reg
class andMemRegAST : public instAST {
    std::string DstMem;
    std::string SrcReg;
    unsigned char size;

public: 
    andMemRegAST(std::string &Mem, std::string &Reg, unsigned char s) 
        : DstMem(Mem), SrcReg(Reg), size(s) {}
};

//and Mem, Imm
class andMemImmAST : public instAST {
    std::string DstMem;
    unsigned long SrcImm;
    unsigned char size;

public: 
    andMemImmAST(std::string &Mem, unsigned long Imm, unsigned char s) 
        : DstMem(Mem), SrcImm(Imm), size(s) {}
};


//=============and done!==================//


//=============or start!==================//

//There are 5 variations. 

//or Reg, Reg
class orRegRegAST : public instAST {
    std::string DstReg;
    std::string SrcReg;
    unsigned char size;

public:
    orRegRegAST(std::string &Reg1, std::string &Reg2, unsigned char s)
        : DstReg(Reg1), SrcReg(Reg2), size(s) {}
};


//or Reg, Mem
class orRegMemAST : public instAST {
    std::string DstReg;
    std::string SrcMem;
    unsigned char size;

public: 
    orRegMemAST(std::string &Reg, std::string &Mem, unsigned char s) 
        : DstReg(Reg), SrcMem(Mem), size(s) {}
};

//or Reg, Imm
class orRegImmAST : public instAST {
    std::string DstReg;
    unsigned long SrcImm;
    unsigned char size;

public: 
    orRegImmAST(std::string &Reg, unsigned long Imm, unsigned char s)
        : DstReg(Reg), SrcImm(Imm), size(s) {}
};

//or Mem, Reg
class orMemRegAST : public instAST {
    std::string DstMem;
    std::string SrcReg;
    unsigned char size;

public: 
    orMemRegAST(std::string &Mem, std::string &Reg, unsigned char s) 
        : DstMem(Mem), SrcReg(Reg), size(s) {}
};


//or Mem, Imm
class orMemImmAST : public instAST {
    std::string DstMem;
    unsigned long SrcImm;
    unsigned char size;

public: 
    orMemImmAST(std::string &Mem, unsigned long Imm, unsigned char s)
        : DstMem(Mem), SrcImm(Imm), size(s) {}
};

//==================or done!========================//





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

    
};

//call Imm
class callImmAST : public instAST {
    unsigned long int Imm;

public:
    callImmAST(unsigned long int I) : Imm(I) {}

    
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

    
};
static rootAST *Root;
//===============root done!=====================//

//====================AST CLASSES DONE=========================//

