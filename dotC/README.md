# The .C() interface

Common wisdom recommends using the .Call() interface.
And further wisdom suggests using Rcpp. However,
for some simple, limited situations, the .C() is the simplest
interface for invoking a routine from R.
It works for specific situations and it involves
working directly with primitive data types for elements (int, double, char *)
and simple pointers in C.
The .Call() requires working with R objects in C code, and Rcpp adds an extra layer
and a new language - C++. So if your problem truly fits into the .C() interface,
there are gains in simplicity. So it is useful to know about it and see how to use it.

In this narrative, we'll use the .C() to do a very simple operation (that R already does better)
just to illustrate the key elements. We'll also show the equivalent .Call() version.

Our task is to compute x + 1 from R using C code.  x is a numeric vector with n elements.
Of course, x + 1 just works in R without needing us to write C code. This is just an example.

If we were to do this in a general purpose C library with not intent to use it in R.,
we might write the C code something like
```
double *
add1(double *x, int len)
{
	double * ans = malloc(len * sizeof(double));
    for(int i = 0; i < len; i++) 
  	   ans[i] = x[i] + 1;
    return(ans);
}
```
This takes the input vector x and the number of elements in the array.
This allocates space for the answer and then loops over the elements of x and adds 1 to that element
and assigns the result to the i-th element of ans.

There are several things about this routine that are not appropriate for using it with R via the
.C() interface.
Firstly, we will pass an R numeric vector (say `input`) as the first argument and that will be marshalled (passed
from R to C) as a double *. So this is what we want. But we also have to pass the number of elements
in the vector as the second argument. We can pass `length(input)`. But this is an integer vector of
length 1. In the same way a numeric vector is passed as double *, an integer vector will be
marshalled
as an int * data type, not an int. This is because 'everything in R is a vector'  and we don't
distinguish between vectors of length n or length 1.
Passing an int *, i.e. the address which contains an integer value, and passing an int which is the
value itself is very different.  If we pass an int with value 2 and the routine expects an int *, it 
will think the integer value is at memory location 2 and try to access that memory cell. This will
cause a crash/segmentation fault.
If we pass an int * - the memory address - and the routine interprets this as an integer, then
we will typically get a very large number corresponding to the memory address and this will be much larger
than the number of elements actually in the vector. It will certainly be unrelated to the number of
elements.

Instead, we need our routine to take an int * for the parameter len
and tou access the value in that pointer/cell with *len in the loop:
```
double *
add1(double *x, double *len)
{
	double * ans = malloc(len * sizeof(double));
    for(int i = 0; i < *len; i++) 
  	   ans[i] = x[i] + 1;
    return(ans);
}
```


Unfortunately, this is still not callable from R. R doesn't understand the return type double *.
Instead, R expects routines called via .C() to return any results by inserting values into
arrays passed as arguments to the routine. <!-- rephrase -->
Instead of allocating ans within the routine and returning it, we will pass a numeric
vector with length the same as x to the routine from R.
Then the routine will fill its elements in. So our routine is actually simpler as
```
void
add1(double *x, double *len, double *ans)
{
    for(int i = 0; i < *len; i++) 
  	   ans[i] = x[i] + 1;
}
```
We call it with
```
x = as.numeric(x)
result = .C("add1", x, length(x), numeric(length(x)))
```
The variable result  is a list containing 3 elements which are the R objects
passed to add1 via the .C().  The first two elements  are x and length(x)
and are of no interest. The third value is the vector created by `numeric(length(x))`
and its contents are the results we want. So we might actually call our add1 routine
with either of
```
.C("add1", x, length(x), numeric(length(x)))[[3]]
.C("add1", x, length(x), ans = numeric(length(x)))$ans
```

So basically, the .C() works well for passing primitive R types - logical, integer, numeric
and character vectors to C as vectors and scalars and as inputs to be read and outputs to be filled
by the C routine.

What about passing matrices? Well a matrix is a vector with attributes. So, for example,
```
m = matrix(1:15, 3, 5)
.C("procMatrix", m, dim(m), numeric(1))
```
passes the matrix as an integer vector and the vector of dimensions  (3, 5) as another
integer vector to procMatix.
It is important to know that matrices are organized as an ordered vector in column-oriented order
so that the matrix
```
     [,1] [,2] [,3] [,4] [,5]
[1,]    1    4    7   10   13
[2,]    2    5    8   11   14
[3,]    3    6    9   12   15
```
is passed as the vector
``` 
[1]  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
```
To get the i,j-th entry in C where i is the row and j is the coulumn,
we need to skip j*nrow() + i cells.  Hence we we need to know the number
of rows when computing this index. And because we are in C, we start counting
at 0.  So the (i = 2, j = 3) - row 2, column 3 - index is 7 corresponding to
(3 - 1)* nrow() + (2-1) = 6 + 1 = 7. And at that position, which is the 8th element
of the vector which starts at position 0, we have the value 8.


What about passing or returning R lists() via .C()? Don't do this.
Use the .Call() interface.

What if our C routine returns a different type of value depending on
the computations, e.g. an integer vector in one case or an numeric vector
in another. If 

