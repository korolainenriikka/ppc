	.file	"testi.cpp"
	.text
	.local	_ZStL8__ioinit
	.comm	_ZStL8__ioinit,1,1
	.globl	main
	.type	main, @function
main:
.LFB1731:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	andq	$-32, %rsp
	subq	$256, %rsp
	movq	%fs:40, %rax
	movq	%rax, 248(%rsp)
	xorl	%eax, %eax
	movl	$0, 60(%rsp)
	jmp	.L2
.L3:
	pxor	%xmm0, %xmm0
	cvtsi2ssl	60(%rsp), %xmm0
	movl	60(%rsp), %eax
	cltq
	movss	%xmm0, 64(%rsp,%rax,4)
	pxor	%xmm0, %xmm0
	cvtsi2ssl	60(%rsp), %xmm0
	movl	60(%rsp), %eax
	cltq
	movss	%xmm0, 96(%rsp,%rax,4)
	addl	$1, 60(%rsp)
.L2:
	cmpl	$7, 60(%rsp)
	jle	.L3
#APP
# 14 "testi.cpp" 1
	addition
# 0 "" 2
#NO_APP
	movq	64(%rsp), %rax
	movq	72(%rsp), %rdx
	movq	%rax, 128(%rsp)
	movq	%rdx, 136(%rsp)
	movq	80(%rsp), %rax
	movq	88(%rsp), %rdx
	movq	%rax, 144(%rsp)
	movq	%rdx, 152(%rsp)
	movq	96(%rsp), %rax
	movq	104(%rsp), %rdx
	movq	%rax, 160(%rsp)
	movq	%rdx, 168(%rsp)
	movq	112(%rsp), %rax
	movq	120(%rsp), %rdx
	movq	%rax, 176(%rsp)
	movq	%rdx, 184(%rsp)
	movaps	128(%rsp), %xmm1
	movaps	160(%rsp), %xmm0
	addps	%xmm0, %xmm1
	movaps	144(%rsp), %xmm2
	movaps	176(%rsp), %xmm0
	addps	%xmm2, %xmm0
	movaps	%xmm1, 16(%rsp)
	movaps	%xmm0, 32(%rsp)
	movq	16(%rsp), %rax
	movq	24(%rsp), %rdx
	movq	%rax, 192(%rsp)
	movq	%rdx, 200(%rsp)
	movq	32(%rsp), %rax
	movq	40(%rsp), %rdx
	movq	%rax, 208(%rsp)
	movq	%rdx, 216(%rsp)
	movl	$0, %eax
	movq	248(%rsp), %rdx
	subq	%fs:40, %rdx
	je	.L5
	call	__stack_chk_fail@PLT
.L5:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1731:
	.size	main, .-main
	.type	_Z41__static_initialization_and_destruction_0ii, @function
_Z41__static_initialization_and_destruction_0ii:
.LFB2226:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	cmpl	$1, -4(%rbp)
	jne	.L8
	cmpl	$65535, -8(%rbp)
	jne	.L8
	leaq	_ZStL8__ioinit(%rip), %rax
	movq	%rax, %rdi
	call	_ZNSt8ios_base4InitC1Ev@PLT
	leaq	__dso_handle(%rip), %rax
	movq	%rax, %rdx
	leaq	_ZStL8__ioinit(%rip), %rax
	movq	%rax, %rsi
	movq	_ZNSt8ios_base4InitD1Ev@GOTPCREL(%rip), %rax
	movq	%rax, %rdi
	call	__cxa_atexit@PLT
.L8:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2226:
	.size	_Z41__static_initialization_and_destruction_0ii, .-_Z41__static_initialization_and_destruction_0ii
	.type	_GLOBAL__sub_I_main, @function
_GLOBAL__sub_I_main:
.LFB2227:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$65535, %esi
	movl	$1, %edi
	call	_Z41__static_initialization_and_destruction_0ii
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2227:
	.size	_GLOBAL__sub_I_main, .-_GLOBAL__sub_I_main
	.section	.init_array,"aw"
	.align 8
	.quad	_GLOBAL__sub_I_main
	.hidden	__dso_handle
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
