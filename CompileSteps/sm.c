#include <stdlib.h>
#include <stdio.h>

extern double sin2(double);

int
main(int argc, char *argv[])
{
    double ans, value;

    // check we have more than one command line argument, the first being the name of the program sm
    if(argc < 2)
	return(1);
    // so we have two or more
    value = atof(argv[1]);
    
    ans = sin2(value);
    printf("%lf\n", ans);
    return(0);
}
