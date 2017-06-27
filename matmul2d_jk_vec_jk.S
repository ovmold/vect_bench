#ifndef N
#error "N is not defined."
#endif
    
	.text
	.p2align 4,,15
	.globl	matmul2d_jk_vec_jk
	.type	matmul2d_jk_vec_jk, @function
matmul2d_jk_vec_jk:
	subq	%rsi, %rdi
	leaq	8192(%rsi), %r8
	leaq	8396800(%rsi), %r10
	leaq	-8192(%rdi), %r9
.L2:
	leaq	(%r9,%r8), %rcx
	leaq	-8192(%r8), %rsi
	movq	%rdx, %rdi
	.p2align 4,,10
	.p2align 3
.L6:
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L3:
	movsd	(%rsi), %xmm0
	mulsd	(%rdi,%rax), %xmm0
	addsd	(%rcx,%rax), %xmm0
	movsd	%xmm0, (%rcx,%rax)
	addq	$8, %rax
	cmpq	$8192, %rax
	jne	.L3
	addq	$8, %rsi
	addq	$8192, %rdi
	cmpq	%rsi, %r8
	jne	.L6
	addq	$8192, %r8
	cmpq	%r8, %r10
	jne	.L2
	rep ret

