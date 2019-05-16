#include <sys/time.h>
#include <sys/resource.h>

#include <Rdefines.h>

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


