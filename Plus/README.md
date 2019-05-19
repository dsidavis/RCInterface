# 

Consider the computation
```
1 + 2
```
in both R and C.

In R, we 
+ read the input from the command line  or file
+ parse the text
+ evaluate the expression
  + find the function + 
     + search along the search path
  + create a call frame
  + match the arguments 1 and 2 to the parameters, using lazy evaluation
  + evaluate the body of the + function in the call frame
  + calls the .Primitive which passes the arguments to C code
  + the C code marshalls the R arguments 
    + evaluates the R expressions - these are simple literals but R objects
	+ considers whether R needs find a method corresponding to a class of either R argument
	+ converts the R values to native/C/primitive representations, i.e. arrays of double or int values
	+ performs the arithmetic - using the appropriate integer- or real-valued operator on the chip
	  + the C code implementing + in R actually handles +, -, * and / so it has to determine
	   which operation is being requested.
	+ converts the result back to an R object, depending on whether it should be numeric or integer
	+ returns to the R function
  + returns the result from the `+` function
  

Now consider the C code
```
int
foo()
{
  return(1+2);
}
```
Firstly, the compiler processes the code before it is ever run.
It could even be compiled by a different person on a different machine
to form an executable program or a routine in a library.

When the overall compiler process (we'll break down the steps later)
sees the code, it recognizes that 1 and 2 are literal values,
knows how to add them and so replaces 1+2 with 3.
It also knows that the result is an int - the C type for a scalar integer.
So the routine is now
```
int
foo()
{
  return(3);
}
```
The cost of the computation is the call to the routine.
And in some cases, the compiler will replace a call `foo()` with the value 3 and
that can lead to further reductions in the number of computations, e.g.
```
 bar( foo() + 1 )
```
becomes `bar(4)`.

We can look at the machine code instructions the compiler generates
for each of the two routines

```
double plus0()
{
    return(1+2);
}

double plus1()
{
    double ans;
    ans = 1;
    ans += 2;
    return(ans);
}
```
The second does the same computations as the first but with a lot more code.
However, the compiler reduces them to the same machine instructions (when using level-2
optimizations -O2). We can see the machine code using the objdump on the compiled "object-level"
code
(or the executable).
You can do this with the shell command
```
make machineCode
```
This will compile plus.o if necessary and then call
```
objdump  -disassemble  plus.o
```
If you reinvoke this command, the plus.o file will not be compiled since it is up-to-date
with respect to plus.c (i.e. plus.o is more recent than plus.c on which it depends).

The code objdump displays in this case is
```
       0:	55 	pushq	%rbp
       1:	48 89 e5 	movq	%rsp, %rbp
       4:	f2 0f 10 05 4c 00 00 00 	movsd	76(%rip), %xmm0
       c:	5d 	popq	%rbp
       d:	c3 	retq
       e:	66 90 	nop
```
and this is essentially the same for plus0 and plus1.
These are CPU-level instructions.
These basically create a routine call, moves the program location,
puts the value 3 into a register as the return value, hands control back
to the caller.
```
_do_arith:
     460:	55 	pushq	%rbp
     461:	48 89 e5 	movq	%rsp, %rbp
     464:	41 57 	pushq	%r15
     466:	41 56 	pushq	%r14
     468:	41 55 	pushq	%r13
     46a:	41 54 	pushq	%r12
     46c:	53 	pushq	%rbx
     46d:	48 83 ec 18 	subq	$24, %rsp
     471:	49 89 ca 	movq	%rcx, %r10
     474:	49 89 d6 	movq	%rdx, %r14
     477:	49 89 f0 	movq	%rsi, %r8
     47a:	48 89 fe 	movq	%rdi, %rsi
     47d:	48 8b 1d 00 00 00 00 	movq	(%rip), %rbx
     484:	48 8b 13 	movq	(%rbx), %rdx
     487:	4c 39 f2 	cmpq	%r14, %rdx
     48a:	74 31 	je	49 <_do_arith+0x5d>
     48c:	49 8b 4e 28 	movq	40(%r14), %rcx
     490:	48 39 d1 	cmpq	%rdx, %rcx
     493:	74 30 	je	48 <_do_arith+0x65>
     495:	48 39 51 28 	cmpq	%rdx, 40(%rcx)
     499:	74 32 	je	50 <_do_arith+0x6d>
     49b:	4c 89 f7 	movq	%r14, %rdi
     49e:	49 89 f4 	movq	%rsi, %r12
     4a1:	4d 89 c5 	movq	%r8, %r13
     4a4:	4d 89 d7 	movq	%r10, %r15
     4a7:	e8 00 00 00 00 	callq	0 <_do_arith+0x4c>
     4ac:	4d 89 fa 	movq	%r15, %r10
     4af:	4d 89 e8 	movq	%r13, %r8
     4b2:	4c 89 e6 	movq	%r12, %rsi
     4b5:	41 89 c7 	movl	%eax, %r15d
     4b8:	48 8b 13 	movq	(%rbx), %rdx
     4bb:	eb 16 	jmp	22 <_do_arith+0x73>
     4bd:	45 31 ff 	xorl	%r15d, %r15d
     4c0:	4c 89 f2 	movq	%r14, %rdx
     4c3:	eb 0e 	jmp	14 <_do_arith+0x73>
     4c5:	41 bf 01 00 00 00 	movl	$1, %r15d
     4cb:	eb 06 	jmp	6 <_do_arith+0x73>
     4cd:	41 bf 02 00 00 00 	movl	$2, %r15d
     4d3:	4d 8b 66 20 	movq	32(%r14), %r12
     4d7:	49 8b 4e 28 	movq	40(%r14), %rcx
     4db:	4c 8b 69 20 	movq	32(%rcx), %r13
     4df:	49 39 54 24 08 	cmpq	%rdx, 8(%r12)
     4e4:	75 06 	jne	6 <_do_arith+0x8c>
     4e6:	49 39 55 08 	cmpq	%rdx, 8(%r13)
     4ea:	74 5c 	je	92 <_do_arith+0xe8>
     4ec:	48 8d 3d cd d9 00 00 	leaq	55757(%rip), %rdi
     4f3:	4c 8d 4d d0 	leaq	-48(%rbp), %r9
     4f7:	48 89 f3 	movq	%rsi, %rbx
     4fa:	4c 89 c2 	movq	%r8, %rdx
     4fd:	4c 89 f1 	movq	%r14, %rcx
     500:	4d 89 c6 	movq	%r8, %r14
     503:	4d 89 d0 	movq	%r10, %r8
     506:	e8 00 00 00 00 	callq	0 <_do_arith+0xab>
     50b:	85 c0 	testl	%eax, %eax
     50d:	74 09 	je	9 <_do_arith+0xb8>
     50f:	4c 8b 65 d0 	movq	-48(%rbp), %r12
     513:	e9 1a 02 00 00 	jmp	538 <_do_arith+0x2d2>
     518:	41 83 ff 01 	cmpl	$1, %r15d
     51c:	48 89 de 	movq	%rbx, %rsi
     51f:	4d 89 f0 	movq	%r14, %r8
     522:	0f 84 f9 01 00 00 	je	505 <_do_arith+0x2c1>
     528:	41 83 ff 02 	cmpl	$2, %r15d
     52c:	0f 85 06 06 00 00 	jne	1542 <_do_arith+0x6d8>
     532:	48 89 f7 	movq	%rsi, %rdi
     535:	4c 89 c6 	movq	%r8, %rsi
     538:	4c 89 e2 	movq	%r12, %rdx
     53b:	4c 89 e9 	movq	%r13, %rcx
     53e:	e8 00 00 00 00 	callq	0 <_do_arith+0xe3>
     543:	e9 e7 01 00 00 	jmp	487 <_do_arith+0x2cf>
     548:	41 83 ff 01 	cmpl	$1, %r15d
     54c:	0f 84 ba 00 00 00 	je	186 <_do_arith+0x1ac>
     552:	41 83 ff 02 	cmpl	$2, %r15d
     556:	0f 85 dc 05 00 00 	jne	1500 <_do_arith+0x6d8>
     55c:	49 8b 3c 24 	movq	(%r12), %rdi
     560:	89 f8 	movl	%edi, %eax
     562:	24 3f 	andb	$63, %al
     564:	3c 2d 	cmpb	$45, %al
     566:	0f 84 1b 01 00 00 	je	283 <_do_arith+0x227>
     56c:	3c 2e 	cmpb	$46, %al
     56e:	75 c2 	jne	-62 <_do_arith+0xd2>
     570:	f2 41 0f 10 44 24 30 	movsd	48(%r12), %xmm0
     577:	49 8b 4d 00 	movq	(%r13), %rcx
     57b:	89 c8 	movl	%ecx, %eax
     57d:	24 3f 	andb	$63, %al
     57f:	3c 2d 	cmpb	$45, %al
     581:	0f 84 bd 01 00 00 	je	445 <_do_arith+0x2e4>
     587:	3c 2e 	cmpb	$46, %al
     589:	75 a7 	jne	-89 <_do_arith+0xd2>
     58b:	48 ba 00 00 00 00 ff ff 00 00 	movabsq	$281470681743360, %rdx
     595:	f2 41 0f 10 4d 30 	movsd	48(%r13), %xmm1
     59b:	4c 89 e0 	movq	%r12, %rax
     59e:	48 85 d7 	testq	%rdx, %rdi
     5a1:	74 37 	je	55 <_do_arith+0x17a>
     5a3:	4c 89 e8 	movq	%r13, %rax
     5a6:	48 85 d1 	testq	%rdx, %rcx
     5a9:	74 2f 	je	47 <_do_arith+0x17a>
     5ab:	83 e7 1f 	andl	$31, %edi
     5ae:	49 89 f6 	movq	%rsi, %r14
     5b1:	be 01 00 00 00 	movl	$1, %esi
     5b6:	31 d2 	xorl	%edx, %edx
     5b8:	4d 89 c7 	movq	%r8, %r15
     5bb:	f2 0f 11 45 c8 	movsd	%xmm0, -56(%rbp)
     5c0:	f2 0f 11 4d c0 	movsd	%xmm1, -64(%rbp)
     5c5:	e8 00 00 00 00 	callq	0 <_do_arith+0x16a>
     5ca:	f2 0f 10 4d c0 	movsd	-64(%rbp), %xmm1
     5cf:	f2 0f 10 45 c8 	movsd	-56(%rbp), %xmm0
     5d4:	4d 89 f8 	movq	%r15, %r8
     5d7:	4c 89 f6 	movq	%r14, %rsi
     5da:	48 89 45 d0 	movq	%rax, -48(%rbp)
     5de:	49 63 48 20 	movslq	32(%r8), %rcx
     5e2:	48 8d 0c 89 	leaq	(%rcx,%rcx,4), %rcx
     5e6:	48 8b 15 00 00 00 00 	movq	(%rip), %rdx
     5ed:	8b 4c ca 10 	movl	16(%rdx,%rcx,8), %ecx
     5f1:	ff c9 	decl	%ecx
     5f3:	83 f9 03 	cmpl	$3, %ecx
     5f6:	0f 87 36 ff ff ff 	ja	-202 <_do_arith+0xd2>
     5fc:	48 8d 15 89 05 00 00 	leaq	1417(%rip), %rdx
     603:	48 63 0c 8a 	movslq	(%rdx,%rcx,4), %rcx
     607:	48 01 d1 	addq	%rdx, %rcx
     60a:	ff e1 	jmpq	*%rcx
     60c:	49 8b 3c 24 	movq	(%r12), %rdi
     610:	89 f8 	movl	%edi, %eax
     612:	24 3f 	andb	$63, %al
     614:	3c 2d 	cmpb	$45, %al
     616:	0f 84 a5 00 00 00 	je	165 <_do_arith+0x261>
     61c:	3c 2e 	cmpb	$46, %al
     61e:	0f 85 fd 00 00 00 	jne	253 <_do_arith+0x2c1>
     624:	49 63 40 20 	movslq	32(%r8), %rax
     628:	48 8d 04 80 	leaq	(%rax,%rax,4), %rax
     62c:	48 8b 0d 00 00 00 00 	movq	(%rip), %rcx
     633:	8b 44 c1 10 	movl	16(%rcx,%rax,8), %eax
     637:	83 f8 01 	cmpl	$1, %eax
     63a:	0f 84 f2 00 00 00 	je	242 <_do_arith+0x2d2>
     640:	83 f8 02 	cmpl	$2, %eax
     643:	0f 85 d8 00 00 00 	jne	216 <_do_arith+0x2c1>
     649:	48 b9 00 00 00 00 ff ff 00 00 	movabsq	$281470681743360, %rcx
     653:	4c 89 e0 	movq	%r12, %rax
     656:	48 85 cf 	testq	%rcx, %rdi
     659:	74 0f 	je	15 <_do_arith+0x20a>
     65b:	83 e7 1f 	andl	$31, %edi
     65e:	be 01 00 00 00 	movl	$1, %esi
     663:	31 d2 	xorl	%edx, %edx
     665:	e8 00 00 00 00 	callq	0 <_do_arith+0x20a>
     66a:	48 89 45 d0 	movq	%rax, -48(%rbp)
     66e:	f2 41 0f 10 44 24 30 	movsd	48(%r12), %xmm0
     675:	66 0f 57 05 63 d7 00 00 	xorpd	55139(%rip), %xmm0
     67d:	66 0f 13 40 30 	movlpd	%xmm0, 48(%rax)
     682:	e9 a8 00 00 00 	jmp	168 <_do_arith+0x2cf>
     687:	45 8b 7c 24 30 	movl	48(%r12), %r15d
     68c:	49 8b 4d 00 	movq	(%r13), %rcx
     690:	89 c8 	movl	%ecx, %eax
     692:	24 3f 	andb	$63, %al
     694:	3c 2d 	cmpb	$45, %al
     696:	0f 84 c4 00 00 00 	je	196 <_do_arith+0x300>
     69c:	3c 2e 	cmpb	$46, %al
     69e:	0f 85 8e fe ff ff 	jne	-370 <_do_arith+0xd2>
     6a4:	48 8b 05 00 00 00 00 	movq	(%rip), %rax
     6ab:	44 3b 38 	cmpl	(%rax), %r15d
     6ae:	0f 84 63 01 00 00 	je	355 <_do_arith+0x3b7>
     6b4:	49 89 f6 	movq	%rsi, %r14
     6b7:	f2 41 0f 2a c7 	cvtsi2sdl	%r15d, %xmm0
     6bc:	e9 64 01 00 00 	jmp	356 <_do_arith+0x3c5>
     6c1:	49 63 40 20 	movslq	32(%r8), %rax
     6c5:	48 8d 04 80 	leaq	(%rax,%rax,4), %rax
     6c9:	48 8b 0d 00 00 00 00 	movq	(%rip), %rcx
     6d0:	8b 44 c1 10 	movl	16(%rcx,%rax,8), %eax
     6d4:	83 f8 01 	cmpl	$1, %eax
     6d7:	74 59 	je	89 <_do_arith+0x2d2>
     6d9:	83 f8 02 	cmpl	$2, %eax
     6dc:	75 43 	jne	67 <_do_arith+0x2c1>
     6de:	41 8b 5c 24 30 	movl	48(%r12), %ebx
     6e3:	48 b8 00 00 00 00 ff ff 00 00 	movabsq	$281470681743360, %rax
     6ed:	48 85 c7 	testq	%rax, %rdi
     6f0:	74 12 	je	18 <_do_arith+0x2a4>
     6f2:	83 e7 1f 	andl	$31, %edi
     6f5:	be 01 00 00 00 	movl	$1, %esi
     6fa:	31 d2 	xorl	%edx, %edx
     6fc:	e8 00 00 00 00 	callq	0 <_do_arith+0x2a1>
     701:	49 89 c4 	movq	%rax, %r12
     704:	4c 89 65 d0 	movq	%r12, -48(%rbp)
     708:	48 8b 05 00 00 00 00 	movq	(%rip), %rax
     70f:	8b 00 	movl	(%rax), %eax
     711:	89 d9 	movl	%ebx, %ecx
     713:	f7 d9 	negl	%ecx
     715:	39 c3 	cmpl	%eax, %ebx
     717:	0f 44 c8 	cmovel	%eax, %ecx
     71a:	41 89 4c 24 30 	movl	%ecx, 48(%r12)
     71f:	eb 11 	jmp	17 <_do_arith+0x2d2>
     721:	48 89 f7 	movq	%rsi, %rdi
     724:	4c 89 c6 	movq	%r8, %rsi
     727:	4c 89 e2 	movq	%r12, %rdx
     72a:	e8 00 00 00 00 	callq	0 <_do_arith+0x2cf>
     72f:	49 89 c4 	movq	%rax, %r12
     732:	4c 89 e0 	movq	%r12, %rax
     735:	48 83 c4 18 	addq	$24, %rsp
     739:	5b 	popq	%rbx
     73a:	41 5c 	popq	%r12
     73c:	41 5d 	popq	%r13
     73e:	41 5e 	popq	%r14
     740:	41 5f 	popq	%r15
     742:	5d 	popq	%rbp
     743:	c3 	retq
     744:	41 8b 45 30 	movl	48(%r13), %eax
     748:	48 8b 0d 00 00 00 00 	movq	(%rip), %rcx
     74f:	3b 01 	cmpl	(%rcx), %eax
     751:	0f 84 46 01 00 00 	je	326 <_do_arith+0x43d>
     757:	f2 0f 2a c8 	cvtsi2sdl	%eax, %xmm1
     75b:	e9 48 01 00 00 	jmp	328 <_do_arith+0x448>
     760:	49 63 40 20 	movslq	32(%r8), %rax
     764:	48 8d 04 80 	leaq	(%rax,%rax,4), %rax
     768:	48 8b 15 00 00 00 00 	movq	(%rip), %rdx
     76f:	8b 44 c2 10 	movl	16(%rdx,%rax,8), %eax
     773:	ff c8 	decl	%eax
     775:	83 f8 03 	cmpl	$3, %eax
     778:	0f 87 b4 fd ff ff 	ja	-588 <_do_arith+0xd2>
     77e:	45 8b 75 30 	movl	48(%r13), %r14d
     782:	48 8d 15 d3 03 00 00 	leaq	979(%rip), %rdx
     789:	48 63 04 82 	movslq	(%rdx,%rax,4), %rax
     78d:	48 01 d0 	addq	%rdx, %rax
     790:	ff e0 	jmpq	*%rax
     792:	48 b8 00 00 00 00 ff ff 00 00 	movabsq	$281470681743360, %rax
     79c:	48 85 c7 	testq	%rax, %rdi
     79f:	74 20 	je	32 <_do_arith+0x361>
     7a1:	4d 89 ec 	movq	%r13, %r12
     7a4:	48 85 c1 	testq	%rax, %rcx
     7a7:	74 18 	je	24 <_do_arith+0x361>
     7a9:	83 e7 1f 	andl	$31, %edi
     7ac:	48 89 f3 	movq	%rsi, %rbx
     7af:	be 01 00 00 00 	movl	$1, %esi
     7b4:	31 d2 	xorl	%edx, %edx
     7b6:	e8 00 00 00 00 	callq	0 <_do_arith+0x35b>
     7bb:	48 89 de 	movq	%rbx, %rsi
     7be:	49 89 c4 	movq	%rax, %r12
     7c1:	4c 89 65 d0 	movq	%r12, -48(%rbp)
     7c5:	48 8b 05 00 00 00 00 	movq	(%rip), %rax
     7cc:	8b 00 	movl	(%rax), %eax
     7ce:	44 39 f8 	cmpl	%r15d, %eax
     7d1:	0f 84 57 03 00 00 	je	855 <_do_arith+0x6ce>
     7d7:	44 39 f0 	cmpl	%r14d, %eax
     7da:	0f 84 4e 03 00 00 	je	846 <_do_arith+0x6ce>
     7e0:	45 85 f6 	testl	%r14d, %r14d
     7e3:	7e 11 	jle	17 <_do_arith+0x396>
     7e5:	b9 ff ff ff 7f 	movl	$2147483647, %ecx
     7ea:	44 29 f1 	subl	%r14d, %ecx
     7ed:	44 39 f9 	cmpl	%r15d, %ecx
     7f0:	0f 8c 93 02 00 00 	jl	659 <_do_arith+0x629>
     7f6:	45 85 f6 	testl	%r14d, %r14d
     7f9:	79 11 	jns	17 <_do_arith+0x3ac>
     7fb:	b9 01 00 00 80 	movl	$2147483649, %ecx
     800:	44 29 f1 	subl	%r14d, %ecx
     803:	44 39 f9 	cmpl	%r15d, %ecx
     806:	0f 8f 7d 02 00 00 	jg	637 <_do_arith+0x629>
     80c:	45 01 fe 	addl	%r15d, %r14d
     80f:	44 89 f0 	movl	%r14d, %eax
     812:	e9 17 03 00 00 	jmp	791 <_do_arith+0x6ce>
     817:	49 89 f6 	movq	%rsi, %r14
     81a:	48 8b 05 00 00 00 00 	movq	(%rip), %rax
     821:	f2 0f 10 00 	movsd	(%rax), %xmm0
     825:	f2 41 0f 10 4d 30 	movsd	48(%r13), %xmm1
     82b:	48 ba 00 00 00 00 ff ff 00 00 	movabsq	$281470681743360, %rdx
     835:	4c 89 e8 	movq	%r13, %rax
     838:	48 85 d1 	testq	%rdx, %rcx
     83b:	74 2b 	je	43 <_do_arith+0x408>
     83d:	83 e1 1f 	andl	$31, %ecx
     840:	be 01 00 00 00 	movl	$1, %esi
     845:	31 d2 	xorl	%edx, %edx
     847:	89 cf 	movl	%ecx, %edi
     849:	4d 89 c7 	movq	%r8, %r15
     84c:	f2 0f 11 45 c8 	movsd	%xmm0, -56(%rbp)
     851:	f2 0f 11 4d c0 	movsd	%xmm1, -64(%rbp)
     856:	e8 00 00 00 00 	callq	0 <_do_arith+0x3fb>
     85b:	f2 0f 10 4d c0 	movsd	-64(%rbp), %xmm1
     860:	f2 0f 10 45 c8 	movsd	-56(%rbp), %xmm0
     865:	4d 89 f8 	movq	%r15, %r8
     868:	48 89 45 d0 	movq	%rax, -48(%rbp)
     86c:	49 63 48 20 	movslq	32(%r8), %rcx
     870:	48 8d 0c 89 	leaq	(%rcx,%rcx,4), %rcx
     874:	48 8b 15 00 00 00 00 	movq	(%rip), %rdx
     87b:	8b 4c ca 10 	movl	16(%rdx,%rcx,8), %ecx
     87f:	ff c9 	decl	%ecx
     881:	83 f9 03 	cmpl	$3, %ecx
     884:	4c 89 f6 	movq	%r14, %rsi
     887:	0f 87 a5 fc ff ff 	ja	-859 <_do_arith+0xd2>
     88d:	48 8d 15 d8 02 00 00 	leaq	728(%rip), %rdx
     894:	48 63 0c 8a 	movslq	(%rdx,%rcx,4), %rcx
     898:	48 01 d1 	addq	%rdx, %rcx
     89b:	ff e1 	jmpq	*%rcx
     89d:	48 8b 05 00 00 00 00 	movq	(%rip), %rax
     8a4:	f2 0f 10 08 	movsd	(%rax), %xmm1
     8a8:	48 b9 00 00 00 00 ff ff 00 00 	movabsq	$281470681743360, %rcx
     8b2:	4c 89 e0 	movq	%r12, %rax
     8b5:	48 85 cf 	testq	%rcx, %rdi
     8b8:	74 2f 	je	47 <_do_arith+0x489>
     8ba:	83 e7 1f 	andl	$31, %edi
     8bd:	49 89 f6 	movq	%rsi, %r14
     8c0:	be 01 00 00 00 	movl	$1, %esi
     8c5:	31 d2 	xorl	%edx, %edx
     8c7:	4d 89 c7 	movq	%r8, %r15
     8ca:	f2 0f 11 45 c8 	movsd	%xmm0, -56(%rbp)
     8cf:	f2 0f 11 4d c0 	movsd	%xmm1, -64(%rbp)
     8d4:	e8 00 00 00 00 	callq	0 <_do_arith+0x479>
     8d9:	f2 0f 10 4d c0 	movsd	-64(%rbp), %xmm1
     8de:	f2 0f 10 45 c8 	movsd	-56(%rbp), %xmm0
     8e3:	4d 89 f8 	movq	%r15, %r8
     8e6:	4c 89 f6 	movq	%r14, %rsi
     8e9:	48 89 45 d0 	movq	%rax, -48(%rbp)
     8ed:	49 63 48 20 	movslq	32(%r8), %rcx
     8f1:	48 8d 0c 89 	leaq	(%rcx,%rcx,4), %rcx
     8f5:	48 8b 15 00 00 00 00 	movq	(%rip), %rdx
     8fc:	8b 4c ca 10 	movl	16(%rdx,%rcx,8), %ecx
     900:	ff c9 	decl	%ecx
     902:	83 f9 03 	cmpl	$3, %ecx
     905:	0f 87 27 fc ff ff 	ja	-985 <_do_arith+0xd2>
     90b:	48 8d 15 6a 02 00 00 	leaq	618(%rip), %rdx
     912:	48 63 0c 8a 	movslq	(%rdx,%rcx,4), %rcx
     916:	48 01 d1 	addq	%rdx, %rcx
     919:	ff e1 	jmpq	*%rcx
     91b:	f2 0f 58 c1 	addsd	%xmm1, %xmm0
     91f:	f2 0f 11 40 30 	movsd	%xmm0, 48(%rax)
     924:	e9 06 fe ff ff 	jmp	-506 <_do_arith+0x2cf>
     929:	f2 0f 59 c1 	mulsd	%xmm1, %xmm0
     92d:	f2 0f 11 40 30 	movsd	%xmm0, 48(%rax)
     932:	e9 f8 fd ff ff 	jmp	-520 <_do_arith+0x2cf>
     937:	f2 0f 5e c1 	divsd	%xmm1, %xmm0
     93b:	f2 0f 11 40 30 	movsd	%xmm0, 48(%rax)
     940:	e9 ea fd ff ff 	jmp	-534 <_do_arith+0x2cf>
     945:	f2 0f 5c c1 	subsd	%xmm1, %xmm0
     949:	f2 0f 11 40 30 	movsd	%xmm0, 48(%rax)
     94e:	e9 dc fd ff ff 	jmp	-548 <_do_arith+0x2cf>
     953:	48 b8 00 00 00 00 ff ff 00 00 	movabsq	$281470681743360, %rax
     95d:	48 85 c7 	testq	%rax, %rdi
     960:	74 20 	je	32 <_do_arith+0x522>
     962:	4d 89 ec 	movq	%r13, %r12
     965:	48 85 c1 	testq	%rax, %rcx
     968:	74 18 	je	24 <_do_arith+0x522>
     96a:	83 e7 1f 	andl	$31, %edi
     96d:	48 89 f3 	movq	%rsi, %rbx
     970:	be 01 00 00 00 	movl	$1, %esi
     975:	31 d2 	xorl	%edx, %edx
     977:	e8 00 00 00 00 	callq	0 <_do_arith+0x51c>
     97c:	48 89 de 	movq	%rbx, %rsi
     97f:	49 89 c4 	movq	%rax, %r12
     982:	4c 89 65 d0 	movq	%r12, -48(%rbp)
     986:	48 8b 05 00 00 00 00 	movq	(%rip), %rax
     98d:	8b 00 	movl	(%rax), %eax
     98f:	44 39 f8 	cmpl	%r15d, %eax
     992:	0f 84 4e 01 00 00 	je	334 <_do_arith+0x686>
     998:	44 39 f0 	cmpl	%r14d, %eax
     99b:	0f 84 45 01 00 00 	je	325 <_do_arith+0x686>
     9a1:	f2 41 0f 2a c6 	cvtsi2sdl	%r14d, %xmm0
     9a6:	45 0f af f7 	imull	%r15d, %r14d
     9aa:	f2 41 0f 2a cf 	cvtsi2sdl	%r15d, %xmm1
     9af:	f2 0f 59 c8 	mulsd	%xmm0, %xmm1
     9b3:	0f 57 c0 	xorps	%xmm0, %xmm0
     9b6:	f2 41 0f 2a c6 	cvtsi2sdl	%r14d, %xmm0
     9bb:	66 0f 2e c8 	ucomisd	%xmm0, %xmm1
     9bf:	0f 85 c4 00 00 00 	jne	196 <_do_arith+0x629>
     9c5:	0f 8a be 00 00 00 	jp	190 <_do_arith+0x629>
     9cb:	41 39 c6 	cmpl	%eax, %r14d
     9ce:	0f 84 b5 00 00 00 	je	181 <_do_arith+0x629>
     9d4:	45 89 74 24 30 	movl	%r14d, 48(%r12)
     9d9:	e9 54 fd ff ff 	jmp	-684 <_do_arith+0x2d2>
     9de:	48 8b 05 00 00 00 00 	movq	(%rip), %rax
     9e5:	8b 00 	movl	(%rax), %eax
     9e7:	44 39 f8 	cmpl	%r15d, %eax
     9ea:	0f 84 03 01 00 00 	je	259 <_do_arith+0x693>
     9f0:	44 39 f0 	cmpl	%r14d, %eax
     9f3:	0f 84 fa 00 00 00 	je	250 <_do_arith+0x693>
     9f9:	f2 41 0f 2a cf 	cvtsi2sdl	%r15d, %xmm1
     9fe:	f2 41 0f 2a c6 	cvtsi2sdl	%r14d, %xmm0
     a03:	f2 0f 5e c8 	divsd	%xmm0, %xmm1
     a07:	f2 0f 11 4d c8 	movsd	%xmm1, -56(%rbp)
     a0c:	e9 f2 00 00 00 	jmp	242 <_do_arith+0x6a3>
     a11:	48 b8 00 00 00 00 ff ff 00 00 	movabsq	$281470681743360, %rax
     a1b:	48 85 c7 	testq	%rax, %rdi
     a1e:	74 20 	je	32 <_do_arith+0x5e0>
     a20:	4d 89 ec 	movq	%r13, %r12
     a23:	48 85 c1 	testq	%rax, %rcx
     a26:	74 18 	je	24 <_do_arith+0x5e0>
     a28:	83 e7 1f 	andl	$31, %edi
     a2b:	48 89 f3 	movq	%rsi, %rbx
     a2e:	be 01 00 00 00 	movl	$1, %esi
     a33:	31 d2 	xorl	%edx, %edx
     a35:	e8 00 00 00 00 	callq	0 <_do_arith+0x5da>
     a3a:	48 89 de 	movq	%rbx, %rsi
     a3d:	49 89 c4 	movq	%rax, %r12
     a40:	4c 89 65 d0 	movq	%r12, -48(%rbp)
     a44:	48 8b 05 00 00 00 00 	movq	(%rip), %rax
     a4b:	8b 00 	movl	(%rax), %eax
     a4d:	44 39 f8 	cmpl	%r15d, %eax
     a50:	0f 84 d8 00 00 00 	je	216 <_do_arith+0x6ce>
     a56:	44 39 f0 	cmpl	%r14d, %eax
     a59:	0f 84 cf 00 00 00 	je	207 <_do_arith+0x6ce>
     a5f:	45 85 f6 	testl	%r14d, %r14d
     a62:	79 0c 	jns	12 <_do_arith+0x610>
     a64:	41 8d 8e ff ff ff 7f 	leal	2147483647(%r14), %ecx
     a6b:	44 39 f9 	cmpl	%r15d, %ecx
     a6e:	7c 19 	jl	25 <_do_arith+0x629>
     a70:	45 85 f6 	testl	%r14d, %r14d
     a73:	0f 8e af 00 00 00 	jle	175 <_do_arith+0x6c8>
     a79:	41 8d 8e 01 00 00 80 	leal	-2147483647(%r14), %ecx
     a80:	44 39 f9 	cmpl	%r15d, %ecx
     a83:	0f 8e 9f 00 00 00 	jle	159 <_do_arith+0x6c8>
     a89:	48 89 f3 	movq	%rsi, %rbx
     a8c:	41 89 44 24 30 	movl	%eax, 48(%r12)
     a91:	4c 8b 35 00 00 00 00 	movq	(%rip), %r14
     a98:	49 63 06 	movslq	(%r14), %rax
     a9b:	48 8b 0d 00 00 00 00 	movq	(%rip), %rcx
     aa2:	3b 01 	cmpl	(%rcx), %eax
     aa4:	0f 8d ad 00 00 00 	jge	173 <_do_arith+0x6f7>
     aaa:	48 8b 0d 00 00 00 00 	movq	(%rip), %rcx
     ab1:	48 8b 09 	movq	(%rcx), %rcx
     ab4:	8d 50 01 	leal	1(%rax), %edx
     ab7:	41 89 16 	movl	%edx, (%r14)
     aba:	4c 89 24 c1 	movq	%r12, (%rcx,%rax,8)
     abe:	48 8d 3d ff d3 00 00 	leaq	54271(%rip), %rdi
     ac5:	e8 00 00 00 00 	callq	0 <_do_arith+0x66a>
     aca:	48 89 c1 	movq	%rax, %rcx
     acd:	31 c0 	xorl	%eax, %eax
     acf:	48 89 df 	movq	%rbx, %rdi
     ad2:	48 89 ce 	movq	%rcx, %rsi
     ad5:	e8 00 00 00 00 	callq	0 <_do_arith+0x67a>
     ada:	41 ff 0e 	decl	(%r14)
     add:	4c 8b 65 d0 	movq	-48(%rbp), %r12
     ae1:	e9 4c fc ff ff 	jmp	-948 <_do_arith+0x2d2>
     ae6:	41 89 c6 	movl	%eax, %r14d
     ae9:	45 89 74 24 30 	movl	%r14d, 48(%r12)
     aee:	e9 3f fc ff ff 	jmp	-961 <_do_arith+0x2d2>
     af3:	48 8b 05 00 00 00 00 	movq	(%rip), %rax
     afa:	f2 0f 10 00 	movsd	(%rax), %xmm0
     afe:	f2 0f 11 45 c8 	movsd	%xmm0, -56(%rbp)
     b03:	bf 0e 00 00 00 	movl	$14, %edi
     b08:	be 01 00 00 00 	movl	$1, %esi
     b0d:	31 d2 	xorl	%edx, %edx
     b0f:	e8 00 00 00 00 	callq	0 <_do_arith+0x6b4>
     b14:	49 89 c4 	movq	%rax, %r12
     b17:	f2 0f 10 45 c8 	movsd	-56(%rbp), %xmm0
     b1c:	f2 41 0f 11 44 24 30 	movsd	%xmm0, 48(%r12)
     b23:	e9 0a fc ff ff 	jmp	-1014 <_do_arith+0x2d2>
     b28:	45 29 f7 	subl	%r14d, %r15d
     b2b:	44 89 f8 	movl	%r15d, %eax
     b2e:	41 89 44 24 30 	movl	%eax, 48(%r12)
     b33:	e9 fa fb ff ff 	jmp	-1030 <_do_arith+0x2d2>
     b38:	48 8d 3d a6 d3 00 00 	leaq	54182(%rip), %rdi
     b3f:	48 89 f3 	movq	%rsi, %rbx
     b42:	e8 00 00 00 00 	callq	0 <_do_arith+0x6e7>
     b47:	48 89 c1 	movq	%rax, %rcx
     b4a:	31 c0 	xorl	%eax, %eax
     b4c:	48 89 df 	movq	%rbx, %rdi
     b4f:	48 89 ce 	movq	%rcx, %rsi
     b52:	e8 00 00 00 00 	callq	0 <_do_arith+0x6f7>
     b57:	e8 00 00 00 00 	callq	0 <_do_arith+0x6fc>
     b5c:	36 fc 	cld
     b5e:	ff ff  <unknown>
     b60:	b5 fe 	movb	$-2, %ch
     b62:	ff ff  <unknown>
     b64:	f7 fd 	idivl	%ebp
     b66:	ff ff  <unknown>
     b68:	82  <unknown>
     b69:	fe ff  <unknown>
     b6b:	ff af fd ff ff d9 	ljmpl	*-637534211(%rdi)
     b71:	fd 	std
     b72:	ff ff  <unknown>
     b74:	bd fd ff ff cb 	movl	$3422552061, %ebp
     b79:	fd 	std
     b7a:	ff ff  <unknown>
     b7c:	9f 	lahf
     b7d:	fd 	std
     b7e:	ff ff  <unknown>
     b80:	c9 	leave
     b81:	fd 	std
     b82:	ff ff  <unknown>
     b84:	ad 	lodsl	(%rsi), %eax
     b85:	fd 	std
     b86:	ff ff  <unknown>
     b88:	bb fd ff ff 8f 	movl	$2415919101, %ebx
     b8d:	fd 	std
     b8e:	ff ff  <unknown>
     b90:	b9 fd ff ff 9d 	movl	$2650800125, %ecx
     b95:	fd 	std
     b96:	ff ff  <unknown>
     b98:	ab 	stosl	%eax, %es:(%rdi)
     b99:	fd 	std
     b9a:	ff ff  <unknown>
     b9c:	0f 1f 40 00 	nopl	(%rax)
```
This is approximately 480 instructions.
This is doing a lot more than the code in our plus0 routine.
But that is part of the problem. We don't need the generality to compute 1+2,
but we pay for it.


We saw compilation, machine code, and the make utility.
