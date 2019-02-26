#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <string.h>
#include <inttypes.h>
#include <capstone/capstone.h>

using namespace std;

int size=0;
int addr=1000;

//This function extracts the .text section from the given elf file
char* get_text(FILE* fd, Elf64_Ehdr eh, Elf64_Shdr sh_table[])
{
    int i;
    char* sh_str;
    char* buff;

    buff = reinterpret_cast<char *>(malloc(sh_table[eh.e_shstrndx].sh_size));

    if(buff != NULL)
    {
        fseek(fd, sh_table[eh.e_shstrndx].sh_offset, SEEK_SET);
        fread(buff, 1, sh_table[eh.e_shstrndx].sh_size, fd);
    }
    sh_str = buff;

    for(i=0; i<eh.e_shnum; i++)
    {
        if(!strcmp(".text", (sh_str + sh_table[i].sh_name)))
        {
            break;
        }
    }

    if (i < eh.e_shnum) {
        char *mydata = reinterpret_cast<char *>(malloc(sh_table[i].sh_size));
        fseek(fd, sh_table[i].sh_offset, SEEK_SET);
        int j=0;
        uint8_t ch;
        while(j<sh_table[i].sh_size){
        
            fread(&ch,1,1,fd); 
            mydata[j++]=(char)ch;
        }
        mydata[j]='\0';
        size = sh_table[i].sh_size;
        addr=sh_table[i].sh_addr;
        return (mydata);
    } else {
        printf("Executable does not contain text segment");
        exit(0);
    }

}

//This function prints the disassembly of the .text section of the given executable
void disass(char* code){

    csh handle;
    cs_insn *insn;
    size_t count;

    if (cs_open(CS_ARCH_X86, CS_MODE_64 , &handle)) {
        printf("ERROR: Failed to initialize engine!\n");
        exit(0);
    }
    
    count = cs_disasm(handle, (unsigned char *)code, size + 1, addr, 0, &insn);
    if (count) {
        size_t j;

        for (j = 0; j < count; j++) {
            printf("0x%"PRIx64":\t%s\t\t%s\n", insn[j].address, insn[j].mnemonic, insn[j].op_str);
        }

        cs_free(insn, count);
    } else
        printf("ERROR: Failed to disassemble given code!\n");

    cs_close(&handle);
}


int main(int argc, char** argv)
{
    FILE* fp = NULL; //Pointer used to access current file
    char* program_name;
    Elf64_Ehdr elf_header; //Elf header
    Elf64_Shdr* sh_table; //Elf symbol table

    if(argc!=2){
        printf("Usage: ./disassemble <executable_name>\n");
        exit(0);
    }

    program_name = argv[1];
    fp = fopen(program_name, "r");

    fseek(fp, 0, SEEK_SET);
    fread(&elf_header, 1, sizeof(Elf64_Ehdr), fp);
    sh_table = reinterpret_cast<Elf64_Shdr*>(malloc(elf_header.e_shentsize*elf_header.e_shnum));

    fseek(fp, elf_header.e_shoff, SEEK_SET);
    fread(sh_table, 1, elf_header.e_shentsize*elf_header.e_shnum, fp);

    char* code=get_text(fp, elf_header, sh_table);
    disass(code);

    return 0;
}
