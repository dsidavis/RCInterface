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


#include <stdio.h>
int
main(int argc, char *argv[])
{
    printf("plus0 = %lf\n", plus0());
    printf("plus1 = %lf\n", plus1());
    return(1);
}
