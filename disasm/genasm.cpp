// genasm.h has routines which helps in generating assembly code of a particular given binary. 
// Using the awesome capstone framework!
// This is the input for the lexer+parser. 

#include<bits/stdc++.h>
#include<capstone/capstone.h>
#include<sys/mman.h>
#include<inttypes.h>
#include<stdio.h>
#include<string.h>

int main(int argc, char **argv) {

    const unsigned char *inst = "\x12\x23\x34\x45\x56\x67\x78\x09";

    csh handle;
    cs_insn insn;

    int major, minor;
    cs_version(&major, &minor);
    printf("Capstone Version: %d.%d\n", major, minor);

    if(cs_open(CS_ARCH_X86, CS_MODE_64,&handle) != CS_ERR_OK) {
        printf("ERROR: Unable to initialize handle\n");
        return -1;
    }

    unsigned long int count, size, address = 0x1234;
/*
    count = cs_disasm(handle,
                    inst,
                    sizeof(inst),
                    0x1234,
                    0,
                    &insn);

*/  
    size = sizeof(inst);
    if(cs_disasm_iter(handle, &inst, &size, &address, &insn) == false) {
        printf("Unable to disassemble :(");
        return -1;
    }

    printf("0x%lx: %s %s\n", insn.address, insn.mnemonic, insn.op_str);

/*  
    for(unsigned long int i = 0; i < count; i++) {

        printf("0x%lx: %s %s\n",insn[i].address, insn[i].mnemonic, insn[i].op_str);
    }
*/
    cs_free(&insn, count);
    cs_close(&handle);

    return 0;
}