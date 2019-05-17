
+ Calling existing C routine from 3rd party library
   + simple routine that takes a vector of doubles and the length and 
   + matrices and accessing the (i,j) element.
 + Compiling 
    + See CompileSteps
    + pre-processor
	   + include -I, 
	+ Makevars
	+ Compile
	+ Link - executable or dynamic library
	   + What is a shared library
	   + Motivation
	   + Benefits - hot-swappable
	   + Drawbacks - finding it at run-time
	   + security limitatins
	      + LD_LIBRARY_PATH/DYLD_LIBRARY_PATH
	+ Load
 + wrapper routine
 + return the result.	
 + Debugging
    + R -d gdb
+ Difference between .C() and .Call()
+ R API
  + Protecting objects
+ Rcpp
+ Doing more in the routine to avoid returning an intermediate object.
   + The externalptr type
   + Releasing memory when eventually no longer used.
   + Making certain not to use it when it has been freed.




# Installing Software Locally

# Configuration
## CMake
## Autoconf
## Make
