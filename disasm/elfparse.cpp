#include<bits/stdc++.h>
#include<stdio.h>
#include<sys/mman.h>
#include<unistd.h>
#include<fcntl.h>
#include<elf.h>
#include<capstone/capstone.h>

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
        fprintf(fdisass, "0x%lx: %s %s\n", insn[i].address, insn[i].mnemonic, insn[i].op_str);
    }

    fclose(fdisass);

    //The following code will aim to dump all ret gadgets present in the text section. 

    FILE *fretgg;
    fretgg = fopen("AllGadgets.dump", "w");
    if(fretgg == NULL) {
        std::cerr<<"Error: Unable to create file to dump the gadgets"<<std::endl;
        return -1;
    }

    unsigned char *ptr = inst;
    unsigned long MovingAddress = EntryAddress;
    unsigned char buffer[N];
    memset(buffer, '\0', sizeof(buffer));
    for(unsigned long i = 0; i < TextSize; i++) {
        unsigned long OldAddress = MovingAddress;
       if(*ptr == 0xc3 || *ptr == 0xcb || *ptr == 0xc2 || *ptr == 0xca) {

           printf("0x%lx\n", MovingAddress);
           
       }

        ptr++;
        MovingAddress++;
    }



    cs_close(&handle);
}



int main(int argc, char **argv) {
    
    if(argc != 3) {
        std::cerr<<"$ "<<argv[0]<<" <BinaryName> <ConfigurableN>"<<std::endl;
        return -1;
    }

    /*
     * 1. Open the Binary
     * 2. Map it onto the memory
     * 3. Find the Entry Address 
     */

    char *BinaryName = argv[1];
    unsigned long SizeofBinary = 0;
    unsigned long EntryAddress, TextAddress;
    unsigned char *start;
    FILE *fp;
    int fd;
    std::string TextSizeStr;
    unsigned long int TextSize;
    unsigned long int ConfN = atoi(argv[2]);

    /*
     * Finding size of the binary. 
     * 1. Using fopen and fclose() - reading the while file once - slow. 
     * 2. Should devise a faster method. 
     */

    fp = fopen(BinaryName, "rb");
    if(fp == NULL) {
        std::cerr<<"Error: Unable to open the file"<<std::endl;
        return -1;
    }

    while(fgetc(fp) != EOF)
        SizeofBinary++;

    fclose(fp);

    /* Done finding size of the binary */

    /* Find Entry Address. 
     * 1. Map the first sizeof(Elf64_Ehdr) bytes(as of now 64-bytes) of the ELF File - Only the ELF_Header. 
     * 2. Find the Entry Address using Elf64_Ehdr->e_entry. 
     * 3. Check if it is pie or normal  = ET_EXEC, ET_DYN
     * 4. Unmap those 64 bytes. 
     */

    fd = open(BinaryName, O_RDONLY, S_IRUSR);
    if(fd == -1) {
        std::cerr<<"Error: Unable to open file while finding entry address"<<std::endl;
        return -1;
    }

    start = (unsigned char *)mmap(NULL,
                SizeofBinary,
                PROT_READ, 
                MAP_PRIVATE, 
                fd,
                0);
    
    if(start == MAP_FAILED) {
        std::cerr<<"Error: Unable the map the ELF header onto memory"<<std::endl;
        return -1;
    }


    Elf64_Ehdr *elf_hdr = (Elf64_Ehdr *)start;
    EntryAddress = (unsigned long int)elf_hdr->e_entry;
    
    if((char)elf_hdr->e_type == ET_EXEC)
        TextAddress = EntryAddress - 0x400000;
    
    else if((char)elf_hdr->e_type == ET_DYN)
        TextAddress = EntryAddress;
    /*
    munmap(start, sizeof(Elf64_Ehdr));
    */
    close(fd);

    /*Done finding the Entry Address */

    /*
     * 1. Finding the size of .text segment. 
     * 2. Using the size command, popen() function. 
     * 3. Parsing the output and getting size. 
     * 4. This is not the right way to be done. 
     * 
     * TODO: Properly parse the ELF file and get the size. 
     * 
     * This is just a stop-gap arragement. 
     */
    
    char command[50];
    sprintf(command, "size %s", BinaryName);

    fp = popen(command, "r");
    if(fp == NULL) {
        std::cerr<<"Error: Unable to execute the command: "<<command<<std::endl;
        return -1;
    }

    char ch = fgetc(fp);
    while(ch == ' ' || isalpha(ch) || ch == '\n' || ch == '\t') {
        ch = fgetc(fp);
    }

    TextSizeStr += ch;
    while(ch != ' ') {
        ch = fgetc(fp);
        TextSizeStr += ch;
    }

    TextSize = std::stoul(TextSizeStr);
    fclose(fp);
    /* Done finding size of .text segment */

    /*
     * Mapping only the .text section onto memory. 
     * 1. As ROP is a code-reuse attack, no other non-executable part has to be mapped onto the memory.
     * 2. The memory mapping is faling. 
     * 3. TODO: Find the reason behind this. 
     */

    //This file descriptor will be open till the end of disassembly. Then close it. 
 /*   fd = open(BinaryName, O_RDONLY, S_IRUSR);
    if(fd == -1) {
        std::cerr<<"Error: Unable to open file - "<<BinaryName<<std::endl;
        return -1;
    }

    //Don't forget to munmap this!
    start = (unsigned char *)mmap(NULL, 
                                TextSize,   //length = TextSize because we have to disassemble only .text section. 
                                PROT_READ,  
                                MAP_PRIVATE, 
                                fd, 
                                TextAddress);   //offset = TextAddress: Disassembly should start from TextAddress. 
    if(start == MAP_FAILED) {
        std::cerr<<"Error: Mapping file to memory failed"<<std::endl;
        return -1;
    }

    printf("start = %p\n", start);
*/

    /*
     * 1. Disassemble the shit out of it!
     */

    //Initialize all variables required for disassembly    

    unsigned char *inst = (unsigned char *)(start + TextAddress);
    if(GetAllGadgets(inst, TextSize, EntryAddress, ConfN) == -1) {
        std::cerr<<"Error: GetGadgets() routine failed for some reason"<<std::endl;
        return -1;
    }
    
    return 0;
}
