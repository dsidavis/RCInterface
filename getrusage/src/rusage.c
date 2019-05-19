#include <sys/time.h>
#include <sys/resource.h>

#include <Rdefines.h>

SEXP mkTimeval(struct timeval v);

//extern "C"
SEXP
R_getrusage(SEXP r_who)
{
    struct rusage usg;
    int status;
    SEXP ans;

    int who = INTEGER(r_who)[0];
    status = getrusage(who, &usg);

    if(status != 0) {
	PROBLEM "getrusage failed with %d", who
	    ERROR;
    }

    PROTECT(ans = NEW_NUMERIC(14));
    double *vals = REAL(ans);
    int i = 0;

    vals[i++] = usg.ru_maxrss;
    vals[i++] = usg.ru_ixrss;
    vals[i++] = usg.ru_idrss;
    vals[i++] = usg.ru_isrss;
    vals[i++] = usg.ru_minflt;
    vals[i++] = usg.ru_majflt;
    vals[i++] = usg.ru_nswap;
    vals[i++] = usg.ru_inblock;
    vals[i++] = usg.ru_oublock;
    vals[i++] = usg.ru_msgsnd;
    vals[i++] = usg.ru_msgrcv;
    vals[i++] = usg.ru_nsignals;
    vals[i++] = usg.ru_nvcsw;
    vals[i++] = usg.ru_nivcsw;
    
    UNPROTECT(1);
    return(ans);
}





SEXP
R_getrusage2(SEXP r_who)
{
    struct rusage usg;
    int status;
    SEXP ans, a;

    int who = INTEGER(r_who)[0];
    status = getrusage(who, &usg);

    if(status != 0) {
	PROBLEM "getrusage failed with %d", who
	    ERROR;
    }

    int np = 0;
    PROTECT(ans = NEW_LIST(16));
    np++; 

    SET_VECTOR_ELT(ans, 0, mkTimeval(usg.ru_utime));
    SET_VECTOR_ELT(ans, 1, mkTimeval(usg.ru_stime));


    int i = 2;    
    SET_VECTOR_ELT(ans, i++, ScalarReal(usg.ru_maxrss));
    SET_VECTOR_ELT(ans, i++, ScalarReal(usg.ru_ixrss));
    SET_VECTOR_ELT(ans, i++, ScalarReal(usg.ru_idrss));
    SET_VECTOR_ELT(ans, i++, ScalarReal(usg.ru_isrss));
    SET_VECTOR_ELT(ans, i++, ScalarReal(usg.ru_minflt));
    SET_VECTOR_ELT(ans, i++, ScalarReal(usg.ru_majflt));
    SET_VECTOR_ELT(ans, i++, ScalarReal(usg.ru_nswap));
    SET_VECTOR_ELT(ans, i++, ScalarReal(usg.ru_inblock));
    SET_VECTOR_ELT(ans, i++, ScalarReal(usg.ru_oublock));
    SET_VECTOR_ELT(ans, i++, ScalarReal(usg.ru_msgsnd));
    SET_VECTOR_ELT(ans, i++, ScalarReal(usg.ru_msgrcv));
    SET_VECTOR_ELT(ans, i++, ScalarReal(usg.ru_nsignals));
    SET_VECTOR_ELT(ans, i++, ScalarReal(usg.ru_nvcsw));
    SET_VECTOR_ELT(ans, i++, ScalarReal(usg.ru_nivcsw));                    	
    
    UNPROTECT(1);
    return(ans);
}



SEXP
mkTimeval(struct timeval v)
{
    SEXP ans =  NEW_NUMERIC(2);
    // Don't have to protect hd
    REAL(ans)[0] = v.tv_sec;
    REAL(ans)[1] = v.tv_usec;
    return(ans);
}
