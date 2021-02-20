# ROP-Rough-Space

This is a place where we put whatever we think related to ROP Compiler

1. **Gadgets**: This has a tool which will collect all ROP Gadgets present in executable section of a specified executable.

2. **IRGen**: Plan was to convert ROP Gadgets into LLVM-IR, then feed it to Z3 to extract semantics. This directory has a simple lexer and parser written covering most common x64 assembly instructions. 

3. **disasm**: Rough Space for Figuring Out Gadget Harvesting & Storage. In this directory, we have written code to store the gadgets in a trie which will help us access them quickly - useful in later stages.

4. **disassembly tool**: A Simple disassembly tool which will give the disass of .text section of an executable. This is a mini-version of objdump. This tool was written by Arpitha Raghunandan.

5. **exploit_with_memory_leak**: We tried to exploit a vulnerable program using ROP with most security features enabled. ASLR and W^X were enabled. Executable was compiled without Stack canaries. The exploitation was not possible with ROP alone. It needed a memory leak vulnerability to get the idea of memory layout of vulnerable process. This directory has vulnerable programs and respective exploit scripts.


