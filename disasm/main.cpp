#include<bits/stdc++.h>
#include<stdio.h>
#include<sys/mman.h>
#include<unistd.h>
#include<fcntl.h>
#include<elf.h>
#include<capstone/capstone.h>

#include "Gadgets.h"
#include "trie.h"

Trie trie;

//TODO: 
//1. To find the size of the file, the program is reading the file and closing it - slow - find a better method. 
//2. To find the size of the text segment, the "size" command is being executed and result is taken from it. We should have our own routines doing it. 
//3. Memory mapping only the text segment is giving an error. So, for now, the whole binary is mapped - memory intensive. Find the reason behind the error and fix it. 


int main(int argc, char **argv) {
    
    if(argc != 3) {
        std::cerr<<"$ "<<argv[0]<<" <BinaryName> <ConfigurableN>"<<std::endl;
        return -1;
    }

    
    // 1. Open the Binary
    // 2. Map it onto the memory
    // 3. Find the Entry Address 

    char *BinaryName = argv[1];
    unsigned long SizeofBinary = 0;
    unsigned long EntryAddress, TextAddress;
    unsigned char *start;
    FILE *fp;
    int fd;
    std::string TextSizeStr;
    unsigned long int TextSize;
    unsigned long int ConfN = atoi(argv[2]);

    
    // Finding size of the binary. 
    // 1. Using fopen and fclose() - reading the while file once - slow. TODO: 2. Should devise a faster method. 

    fp = fopen(BinaryName, "rb");
    if(fp == NULL) {
        std::cerr<<"Error: Unable to open the file"<<std::endl;
        return -1;
    }

    while(fgetc(fp) != EOF)
        SizeofBinary++;

    fclose(fp);

    // Done finding size of the binary //

    // Find Entry Address // 
    
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

    // Done finding the Entry Address //

    
    //1. Finding the size of .text segment. 
    //2. Using the size command, popen() function. 
    //3. Parsing the output and getting size. 
    //4. This is not the right way to be done. 
     
    //TODO: Properly parse the ELF file and get the size. 
     
    //5. This is just a stop-gap arragement. 
     
    
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
    // Done finding size of .text segment //

    //=======Disassemble the shit out of it! LOLLLL=========//
    unsigned char *inst = (unsigned char *)(start + TextAddress);
    if(GetAllGadgets(inst, TextSize, EntryAddress, ConfN) == -1) {
        std::cerr<<"Error: GetGadgets() routine failed for some reason"<<std::endl;
        return -1;
    }

    printf("IN MAIN!\n");
    std::vector<cs_insn> hope = gadgets[1];
    for(int k=0; k< hope.size(); k++) {
        for(int l=0; l < hope[k].size;l++) {
            printf("%x ", hope[k].address);
        }
        printf("\n");
    }

    // Vectors that will be used to build the trie
    std::vector<std::vector< std::vector< uint8_t > > > trie_gadgets;
    std::vector<std::vector<uint64_t > > trie_addresses;


    for(int i=0 ; i < gadgets.size(); i++) {
        trie_gadgets.push_back(std::vector<std::vector<uint8_t >>());
        std::vector<uint64_t> addresses;
        for(int j =0; j < gadgets[i].size() ; j++) {
            std::vector<uint8_t> temp;
            uint64_t address = gadgets[i][j].address;
            
            for(int k=0 ; k < gadgets[i][j].size; k++) {
                temp.push_back(gadgets[i][j].bytes[k]);
            }
            trie_gadgets[i].push_back(temp);
            addresses.push_back(address);
        }
        trie_addresses.push_back(addresses);
    }

    // For debugging purposes, to see if the gadgets are being stored correctly
    // for each gadget
    for(int i=0; i < trie_gadgets.size();i++) {
        printf("Gadget #%d\n", i);
        // for each insn in a particular gadget
        for(int j=0 ; j < trie_gadgets[i].size(); j++) {
            // for each byte in a particular insn
            for(int k=0 ; k < trie_gadgets[i][j].size(); k++) {
                printf("0x%x: %x ", trie_addresses[i][j] ,trie_gadgets[i][j][k]);
            }
            printf("\n");
        }
    }

    trie.build_trie(trie_gadgets, trie_addresses, trie_gadgets.size());


    // For debugging: checking if build_trie worked. Search for a gadget that's supposed to be there
    // In this example, 00 5d c3 is being searched for.
    // for ./elfparse hello 5, this should be gadget #2
    bool result = false;
    printf("%x ", trie.search({{0,0}, {93}, {195}}, result));
    if(result==1) {
        cout << "Gadget is present in trie" << endl;
    } else {
        cout << "Gadget is not present in trie" << endl;
    }
    
    return 0;
}
