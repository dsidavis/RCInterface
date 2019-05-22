# Working with C/C++ in R
### Duncan Temple Lang
### Data Science Initiative, UC Davis

This workshop provides an overview and introduction to the 
ecosystem for interfacing R with compiled code, specifically 
C and C++. (These cover other compiled languages and also provide the
lowest common denominator for all languages, including interpreted languages
which can be embedded in R and vice versa)

We discuss 
+ the nature of compiled code, 
+ the "compilation" process and the importance of understanding the different steps
  and the make utility for managing dependencies and keeping files updated with respect
  to changes in others.
+ calling a simple routine working on a primitive R vector
+ using the .Call() interface for more complex and flexible situations, 
  e.g. creating R objects, specifically lists in C code.
+ writing an R package with compiled code
+ interfacing to the 3rd party compiled libraries in C and C++
+ machine-specific compilation flags using pkg-config at installation time
+ configuration for machines with autoconf before compiling.
+ external pointers in R for C/C++ objects (across R function calls)


## [Slides](slides.html)

<!-- ### [R Session]() -->


## [Plus](Plus/README.md)
+ Why compiled code is faster than interpreted code
+ Understanding what a compiler does and when.
+ dissambler and machine code
+ the make utiity

## [CompileSteps](CompileSteps/README.md)
+ Steps in the "compilation" process
  + Pre-processor
  + compiler
  + linker
     + executable
	 + shared library
  + loading

## [dotC](dotC)
+ .C() interface to simple C routine taking a numeric vector
+ .Call() version

## getrusage
+ interface to existing system-level C routine
  + returns a structure
+ .Call()
+ R's C API

## [Rqpdf](Rqpdf)
+ C & C++, 
+ .Call()
+ pkg-config for compiler and library flags for 3rd party software,
+ external pointers to C/C++ objects in R
  + finalizers and garbage collection

