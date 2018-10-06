;This is 1 ret gadget for which I will be writing a parser. 

label1:
dec r13
mov rax, 0x00
push 1234
pop rbx
push r8
inc rsp
pop rax
dec al
sub eax, 2345

label2: 
mov rsp, rbp
push r13
inc ax
add rsi, 0x209030945
mov rbp, rsp
push 0x2345
add al, 0x4444
pop r15
inc esp
mov r15b, r16b
