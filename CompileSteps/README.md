# Steps in "Compilation"

We talk generally and broadlh about compiling code. However, there are several
steps. And errors can occur in each of these. It is important to be able to 
determine in which step a problem occurred so that you can correct it quickly.

There are basically 4 steps to dealing with compiled code after the code has been written.
And of course, we typically iterate through  modifying the code, compiling it and testing it.
These 4 steps in compilation are
+ Pre-processing
+ compiling
+ linking
+ loading

Preprocessing is a step before compilation that includes code from other central, reusable files
and performs text manipulation to expand and rewrite the input code.

Compiling converts the code in one file into low-level machine instructions.
However, it doesn't create an executable program.

Linking involves combining compiled code from two or more files and/or libraries that were 
compiled separately, or a single file of code and making it into an executable. Linking resolves routines and variables (symbols) referenced in one piece of
code and defined in another piece of code.
The result is an executable or a dynamically loadable/shared library (DLL/DSL) or a static library
for later use in linking.

Loading is either running an executable program or loading a dynamic library (a DSO or DLL)
into an existing process, e.g., an R session and loading compiled code for an R package.
Loading involves finding the other DLLs on which this DLL or executable depends and "includes".



## Pre-Processor


We start with the simple C file simpleMath.c
```
#include <math.h>

double
sin2(double x)
{
    return( sin(2 * x) );
}
```
This is intentionally very simple.
We define sin2 routine to take a single scalar real value (double in C).
The routne multiplies this input by 2 and then passes the result to the sin()
routine and returns the result.

The multiplication operator is built into the compiler and the CPU.
However, the sin() routine is not. It is provided in a library of compiled routines
that other compiled code can use.
sin() and other mathematical routines are provided in the math library.
This is compiled code in a file named libm.a or libm.so.
That is machine code that we saw in the [Plus directory](Plus/Readme.md).
libm.a doesn't actually have the signature - the number and types of the parameters/inputs and the
type of  the output - of the sin() routine in it in a readily accessible manner.
Instead, the **declaration** (not the definition) of the sin() and other routines are in
the header file math.h. (The h is for header and is a convention for C code headers.)
The declaration of these routines is in a header file so that we don't have to repeat it in all
of our C code. This avoids redundancy, but also ensures that we don't make a mistake
and declare these routines as taking different types of parameters or returning a different type 
than the actual definition. Unlike R, the types in compiled code are very important and rigid.
Passing an int rather than a long (an integer value that can represent a larger range of integers)
or an int rather than a double (a real valued number) will at best cause the program to crash
(a segmentation fault) or worse, silently give you wrong answers.

The file simpleMath.c has the line 
```
#include <math.h>
```
near the top. The intent is reasonably clear - substitute the line
`#include <math.h>` with a copy of the entire contents of the math.h
file. While it may not be important, it is the C/C++ pre-processor that
reads the simpleMath.c file and processes the #include and other pre-processor
directives (e.g., the #define) to create a new file. 
When compiling this file, we call the C compiler, but it first calls the pre-processor
and gets its output which it then compiles.
It can be useful to see the contents/output of the pre-processor work.
We could run the pre-processor ourselves or equivalently tell the compiler to stop after running
the pre-processor, i.e. don't do the compilation.
We can typically do the latter with
```
cc -E simpleMath.c
```
or
```
cc -E simpleMath.c -o simpleMath.E
```
You can do this with the shell command `make simpleMath.E` in this directory.

Now let's look at the top of the simpleMath.E file.
On my machine, it looks like
```
# 1 "simpleMath.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 361 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
...
...
double
sin2(double x)
{
    return( sin(2 * x * 3.141593) );
}
```
The pre-processor has copied the contents of math.h into the file, added some other information
and finally we see our routine sin2().  Note that the line `#define PI ...` is not present
and the use of PI has been replaced with the text we defined PI with - 3.141593


The header file math.h is often located in /usr/include/ or /usr/local/include along with other header
files. The pre-processor knows to look in several standard places.
However, we often will want to `#include` files that are in other, non-standard places.
We don't want to hard-code the full path to the file in the `#include`, e.g.
```
// BAD
#include </Users/duncan/Project/R-3.6.0/include/Rdefines.h>
```
This won't run on my other machines where Rdefines.h is in a different location,
or with a different version of R. And, of course, it won't run on your computer since
you are very unlikely to have a /Users/duncan directory!

So we need to be able to give the pre-processor one or more additional directories in which 
it will look for header files mentioned in `#include` statements.
We do this with the `-I/path/to/dir` command line argument for the compiler (which passes it to the
preprocessor).
We will see how we use this in the [Rqpdf](../Rqpdf/README.md) directory.


## Compilation

After the preprocessor has created the input for the compiler,
the compiler analyses that file and performs many clever rewrites of the
code to optimize it - do the equivalent computations but in perhaps different ways
and different orders to make it faster, use less memory, and/or less power/electricity.

The result of the compilation step is an object code file.
It contains the low-level CPU instructions that implement the computations
in each routine in the source code file.
The instructions are specific to the targetted type of CPU and also the operating system.
So the same code won't run on a Mac and a PC, or on Linux and a Mac, or on an iPhone 
and GPU. Each compiler will generate slightly different code, and a single compiler
will generate different machine/object code depending on the arguments we pass to the 
compiler, e.g., the level of compiler optimizations to perform -O0, -O2, -O3, etc.

Let's compile simpleMath.c with
```
make simpleMath.o
```
Again, we can look at the object code with objdump
```
objdump -disassemble simpleMath.o
```
On my Mac (OSX), I get
```

simpleMath.o:	file format Mach-O 64-bit x86-64

Disassembly of section __TEXT,__text:
_sin2:
       0:	55 	pushq	%rbp
       1:	48 89 e5 	movq	%rsp, %rbp
       4:	48 83 ec 10 	subq	$16, %rsp
       8:	f2 0f 10 0d 18 00 00 00 	movsd	24(%rip), %xmm1
      10:	f2 0f 11 45 f8 	movsd	%xmm0, -8(%rbp)
      15:	f2 0f 59 4d f8 	mulsd	-8(%rbp), %xmm1
      1a:	0f 28 c1 	movaps	%xmm1, %xmm0
      1d:	e8 00 00 00 00 	callq	0 <_sin2+0x22>
      22:	48 83 c4 10 	addq	$16, %rsp
      26:	5d 	popq	%rbp
      27:	c3 	retq
```
Don't worry about making sense of the instructions.
We see the specific details of the target CPU - 64 bit x86.
The sin2() routine is the only routine in the file since that
was the only one defined (as opposed to declared in the header file).


If I compile this with level-2 optimization (as opposed to no optimization) with
```
rm simpleMath.o
make simpleMath.o CFLAGS=-O2
```
the resulting file is
```

simpleMath.o:	file format Mach-O 64-bit x86-64

Disassembly of section __TEXT,__text:
_sin2:
       0:	55 	pushq	%rbp
       1:	48 89 e5 	movq	%rsp, %rbp
       4:	f2 0f 58 c0 	addsd	%xmm0, %xmm0
       8:	f2 0f 59 05 08 00 00 00 	mulsd	8(%rip), %xmm0
      10:	5d 	popq	%rbp
      11:	e9 00 00 00 00 	jmp	0 <_sin2+0x16>
```
There are fewer instructions.
The compiler has reduced the `2*x*PI` to `6.283185*x`
via constant-folding.

## Linking

Suppose we wanted to create a program that we could call at the command-line
to compute sin2() for a given input. 
To do this, we have to have a main() routine. That is the starting point
for each executable, the routine that is called when we run that executable.
We can add this routine to simpleMath.c, but instead, we'll define it in
a separate file named [sm.c](sm.c).
We expect this to be called from the command line with, e.g.,
```
sm 1.5
```
Don't worry about the details of the main() routine. Basically, it takes
an array/vector of strings (argv) passed to it from the command line when it was called
and also the number of these arguments (argc), including the name of the executable as the first
of these arguments. In our call above, argc = 2, and argv is "sm" and "1.5"
The main routine converts the one argument ("1.5") to a real-valued number using
the routine atof() and then calls our sin2() routine.
It writes the answer to stdout via a call to printf() and then exits with 
status code 0 - the default for success/okay/no error.


From our discussions above about the pre-processor, you know that we need
to declare atof() and we do  that via `#include` of the relevant header file.
atof() is declared in stdlib, denoting standard library.
We also need the declaration for printf() and that is in stdio (standing for standard input and
output).

We don't have a header file that declares our sin2() routine. 
We probably should create it. But for now, we'll copy the signature from
simpleMath.c and insert it directly into this sm.c file.

We compile sm.o with
```
make sm.o
```
sm.o has a call to sin2(), but it doesn't yet know 
where sin2() actually is.  We have to link or integrate sm.o and simpleMath.o.

We call the linker to create our executable sm from sm.o and simpleMath.o by again
calling the compiler. It recognizes that the input files are compiled object/machine-code files and
so calls the linker.  

The linker resolves all the uses/references of all the symbols and connects those
to where those symbols are defined.
It combines all of the code into a single file with the symbols resolved.
So the call to sin2() in main() is connected to the definition taken from simpleMath.o.

We still have  calls to sin(), atof() and printf() that need to be resolved.
Where are these defined?
These are in libraries of compiled code that are on our system for all compiled code to use.
sin() is in the math library. The file is libm.a or libm.so or some variation of that.
atof() and printf() are in another standard library that the linker automatically searches for 
code.
We need to tell the linker to look in libm.a/so when it is resolving references to symbols.
We do this for libraries *not* by passing the library as a file, but telling the linker
to look for libm.a or libm.so.  We use the command-line flag -lm to represent libm.a or libm.so
or other variants of the extension.
Similarly, if we want to link with the library named libBob.a or libBob.so, we would use -lBob.
Case is important. 

We also may need to tell the linker where to find a library.
We do this via the -L command line flag, e.g.,
`-L/usr/local/lib` or `-L/Users/duncan/Projects/R-3.6.0/bin/lib`
Unlike for header files, since this is a command-line argument to the linker, we do hard-code paths.
We do however, typically parameterize this in Make files so that it is easily changed
for different machines and setups.

In the case of libm.a, the linker already knows where to find the file and so we don't need to
specify a directory path via the -L flag. We just add -lm to the call to the compiler
which invokes the linker.
```
make sm
```

## Creating Libraries of Compiled Code

Suppose we wanted to package our sin2() routine so that it could be used
as part of a library, just like we used sin().
This is different from using it in an executable. Instead, we want to create
a new library, named, say libsimpleMath.so, and then other code would link against
that. We may have more than the sin2() routine, but several that are generally useful.
First, we would create a header (.h) file with the declarations of our routines.
Others would `#include` this in their code to declare our routines.
Then we would create libsimpleMath.so.
We do this with
```
make libsimpleMath.so
```
This calls the compiler with
```
$(CC) -shared -o libsimpleMath.so simpleMath.o  -lm
```
The -o flag indicates where to write the resulting library the linker produces.
The -shared indicates that this is a dynamically loadable library (DLL/DSO).
We can also create statically linked libraries. We'll discuss the difference later.

<!--
We are combining both simpleMath.o and sm.o  here. However, the sm.o file
only contains the main() routine which is not very useful for other people.
-->

Calling the file command on libsimpleMath.so gives
```
libsimpleMath.so: Mach-O 64-bit dynamically linked shared library x86_64
```

Now that we have a library containing sin2(), we could have built the sm executable
differently. We'll create sm2 by linking sm.o and libsimpleMath.so and libm.so.
```
make sm2
```
and this calls
```
	$(CC) -o $@ sm.o -lsimpleMath -lm
```
ensuring sm.o and libsimpleMath.so are up-to-date.



<!-- 
otool -L/ldd
Finding the shared libraries at load time.
-->


## Static versus Dynamically Loadable Libraries

There are several advantages and disadvantages to linking sm2 to libsimpleMath.so 
rather than combining sm.o and simpleMath.so.
One is that we can update libsimpleMath.so and then sm2 will automatically use
that updated version, even without us recompiling sm2 or any other executable or library
linked to simpleMath.so.
For example, suppose we change simpleMath.c to compute
```
sin( x * PI/ 180)
```
rather than
```
sin( 2 * x * PI)
```

We recompile simpleMath.o and simpleMath.so.
Ordinarily we would modify simpleMath.c, but just for reasons related to writing
this down and making it reproducible within this evolving narrative, we copy
simpleMath.c to simpleMath2.c and modify that file.
Then we create libsimpleMath.so with
```
cc -shared -o libsimpleMath.so simpleMath2.o -lm
```

Now we invoke
```
sm2 .21
```
and we get a different answer  0.003665
compared to 0.968583 with the original version.

The key concept here is that we didn't recompile sm2.
Instead, when we recompiled libsimpleMath.so and invoked sm2,
the system recognized that sm2 dynamically loaded libsimpleMath.so.
We can check this with 
```
ldd sm2 
otool -L sm2   # OSX
```
Now when sm2 is invoked the system searches for these dynamically loadable libraries.
It finds the new libsimpleMath.so and loads that. Hence we get the new implementation of sin2().

Consider the many programs that link against widely used libraries, e.g libm.so.
When we fix a bug in libm.so or provide a faster implementation for a routine, we don't
have to recompile all of those many programs. The updated version of libm.so is automatically
loaded. This is very convenient for system administrators.

Of course, if we change libsimpleMath.so to remove sin2(), then we will end up with an error.
So we assume only the implementation of the routines in libsimpleMath.so change.

A second issue is that when the system loads sm2 and then libsimpleMath.so, it has
to dynamically resolve the symbols referenced in sm2 (i.e. sin2, atof, printf) 
by looking in the dynamically loaded libraries. This is quick, but still has some overhead
that is avoided when we resolve those symbols at link time, not load time.

<!-- 
Hot swapping an update to libsimpleMath
-->



