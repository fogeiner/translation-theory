.READFORMAT:
    .string "%d\n"
.PRINTFORMAT:
    .string "%d"
.globl main

main:
	pushl %ebp
	movl %esp, %ebp

	movl $1, %eax
	pushl %eax

	movl $.PRINTFORMAT, %eax
	pushl %eax

	call printf
	popl %eax
	popl %eax
	popl %ebp
	ret

