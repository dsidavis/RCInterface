#include <Rdefines.h>

SEXP
R_add1_2(SEXP x)
{
    int len = LENGTH(x) ; //Rf_length(x);
    SEXP ans;
    if(TYPEOF(x) != REALSXP) {
	PROBLEM "wrong type"
	    ERROR;
    }
    ans = NEW_NUMERIC(len);
    for(int i = 0; i < len; i++) 
	REAL(ans)[i] = REAL(x)[i] + 1;
    return(ans);
}
