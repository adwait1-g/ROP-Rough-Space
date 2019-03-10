section .data

str: db "/bin//sh", 0x00

section .text
	global _start


_start: 

	;mov rax, 11
	;lea rbx, [str]
	;mov rcx, 0
	;mov rdx, 0
	;int 0x80
	
	mov rax, 59
	lea rbx, [str]
	mov rcx, 0
	mov rdx, 0
	syscall


exit: 
	mov rax, 1
	mov rbx, 0
	int 0x80
