.LC0:
	.string	"%d"
.LC1:
	.string	"%d\n"
	.globl	main
main:
	pushl	%ebp
	movl	%esp, %ebp

	subl	$32, %esp

	leal	28(%esp), %eax
	pushl %eax
	pushl $.LC0
	call	scanf
	popl %eax
	popl %eax

	movl	28(%esp), %eax
	pushl %eax
	pushl $.LC1
	call	printf
	popl %eax
	popl %eax

	movl	$0, %eax
	leave
	ret
