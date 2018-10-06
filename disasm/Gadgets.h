#include<bits/stdc++.h>
#include<stdio.h>
#include<sys/mman.h>
#include<unistd.h>
#include<fcntl.h>
#include<elf.h>
#include<capstone/capstone.h>


// GetAllGadgets: A Stub function in between low-level functions(functions which actually disassemble stuff) and the main function. 
int GetAllGadgets(unsigned char *inst, unsigned long TextSize, unsigned long EntryAddress, unsigned long N) {

    //Disassembly of the text section will go into a file named disass.dump. 
    FILE *fdisass;
    fdisass = fopen("disass.dump", "w");
    if(fdisass == NULL) {
        std::cerr<<"Error: Unable to create file to dump disassembly"<<std::endl;
        return -1;
    }
    

    //Capstone related variables. 
    csh handle;
    cs_insn *insn;
    unsigned long InstCount;

    //Initialize the handle
    if(cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) {
        std::cerr<<"Error: Unable to open cs handle"<<std::endl;
        return -1;
    }
    
    InstCount = cs_disasm(handle,
                        inst,
                        TextSize,
                        EntryAddress,
                        0,
                        &insn);
    
    if(InstCount == 0) {
        std::cerr<<"Error: Unable to disassemble the .text section"<<std::endl;
        return -1;
    }

    for(unsigned long i = 0; i < InstCount; i++) {
        fprintf(fdisass, "0x%lx: %s %s %s\n", insn[i].address, insn[i].bytes, insn[i].mnemonic, insn[i].op_str);
    }

    fclose(fdisass);

    //The following code will aim to dump all ret gadgets present in the text section. 

    FILE *fretgg;
    fretgg = fopen("AllGadgets.dump", "w");
    if(fretgg == NULL) {
        std::cerr<<"Error: Unable to create file to dump the gadgets"<<std::endl;
        return -1;
    }

    unsigned char *ptr1 = inst, *ptr2 = inst;
    unsigned long MovingAddress = EntryAddress, count = 0;
    unsigned char buffer[N];
    memset(buffer, '\0', sizeof(buffer));
/*
    for(unsigned long i = 0; i < TextSize; i++) {
        
        if(*ptr1 == 0xc3 || *ptr1 == 0xc2 || *ptr1 == 0xcb || *ptr1 == 0xca) {
            printf("count = %lu, Moving Address = 0x%lx\n", count, MovingAddress);
            count++;
        }
        ptr1++;
        MovingAddress++;
    }
*/

     for(unsigned long i = 0; i < TextSize; i++) {
         unsigned long OldAddress = MovingAddress;
        if(*ptr2 == 0xc3 || *ptr2 == 0xcb || *ptr2 == 0xc2 || *ptr2 == 0xca) {

            printf("\nCount = %lu, Address: 0x%lx, rettype = %x\n", count, MovingAddress, *ptr1);
            GetGadgets(ptr2, MovingAddress, N);
            count++;
        }

         ptr2++;
         MovingAddress++;
     }



    cs_close(&handle);
}


