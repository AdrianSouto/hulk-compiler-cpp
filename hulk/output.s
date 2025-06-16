	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"hulk_module"
	.def	main;
	.scl	2;
	.type	32;
	.endef
	.text
	.globl	main                            # -- Begin function main
	.p2align	4
main:                                   # @main
.seh_proc main
# %bb.0:                                # %entry
	pushq	%rbp
	.seh_pushreg %rbp
	pushq	%rsi
	.seh_pushreg %rsi
	pushq	%rdi
	.seh_pushreg %rdi
	pushq	%rbx
	.seh_pushreg %rbx
	subq	$56, %rsp
	.seh_stackalloc 56
	leaq	48(%rsp), %rbp
	.seh_setframe %rbp, 48
	.seh_endprologue
	callq	__main
	movl	$42, 4(%rbp)
	leaq	.L__unnamed_1(%rip), %rdi
	movq	%rdi, -8(%rbp)
	movl	$256, %ecx                      # imm = 0x100
	callq	malloc
	movq	%rax, %rsi
	movq	%rax, %rcx
	movq	%rdi, %rdx
	callq	strcpy
	movl	$32, %ecx
	callq	malloc
	movq	%rax, %rdi
	leaq	.L__unnamed_2(%rip), %rdx
	movq	%rax, %rcx
	movl	$42, %r8d
	callq	sprintf
	movq	%rsi, %rcx
	movq	%rdi, %rdx
	callq	strcat
	movq	%rdi, %rcx
	callq	free
	leaq	.L__unnamed_3(%rip), %rcx
	movq	%rsi, %rdx
	callq	printf
	movq	-8(%rbp), %rsi
	movl	4(%rbp), %edi
	movl	$256, %ecx                      # imm = 0x100
	callq	malloc
	movq	%rax, %rbx
	movq	%rax, %rcx
	movq	%rsi, %rdx
	callq	strcpy
	movl	$32, %ecx
	callq	malloc
	movq	%rax, %rsi
	leaq	.L__unnamed_4(%rip), %rdx
	movq	%rax, %rcx
	movl	%edi, %r8d
	callq	sprintf
	movq	%rbx, %rcx
	movq	%rsi, %rdx
	callq	strcat
	movq	%rsi, %rcx
	callq	free
	xorl	%eax, %eax
	addq	$56, %rsp
	popq	%rbx
	popq	%rdi
	popq	%rsi
	popq	%rbp
	retq
	.seh_endproc
                                        # -- End function
	.section	.rdata,"dr"
.L__unnamed_1:                          # @0
	.asciz	"The meaning of life is"

.L__unnamed_2:                          # @1
	.asciz	"%d"

.L__unnamed_3:                          # @2
	.asciz	"%s\n"

.L__unnamed_4:                          # @3
	.asciz	"%d"

