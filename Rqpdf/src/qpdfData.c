#include <qpdf/qpdf-c.h>
#include <Rdefines.h>

// external pointer for qpdf_data object

void
R_qpdf_data_cleanup(SEXP rval)
{
    void *val = R_ExternalPtrAddr(rval);
    if(val) {
	Rprintf("calling qpdf_cleanup for %p\n", val);
	qpdf_cleanup((qpdf_data *) &val);
    }
}

SEXP
R_qpdf_data_init()
{
    qpdf_data tmp = qpdf_init();
    SEXP rans;
    rans = R_MakeExternalPtr(tmp, Rf_install("qpdf_data"), R_NilValue);
    PROTECT(rans);
    R_RegisterCFinalizer(rans, R_qpdf_data_cleanup);
    UNPROTECT(1);
    return(rans);
}

SEXP
R_qpdf_data_read(SEXP r_pdf, SEXP r_filename)
{
    qpdf_data pdf = (qpdf_data) R_ExternalPtrAddr(r_pdf);
    QPDF_ERROR_CODE err = qpdf_read(pdf, CHAR(STRING_ELT(r_filename, 0)), NULL);
    if(err != QPDF_SUCCESS) {
	PROBLEM "failed to read %s", CHAR(STRING_ELT(r_filename, 0))
	    ERROR;
    }
    return(R_NilValue);
}

SEXP
R_qpdf_data_info(SEXP r_pdf, SEXP r_key)
{
    qpdf_data pdf = (qpdf_data) R_ExternalPtrAddr(r_pdf);
    char const * ans = qpdf_get_info_key(pdf, CHAR(STRING_ELT(r_key, 0)));
    return( ScalarString(ans ? mkChar(ans) : R_NaString) ); 
}

SEXP
R_qpdf_data_version(SEXP r_pdf)
{
    qpdf_data pdf = (qpdf_data) R_ExternalPtrAddr(r_pdf);
    char const * ans = qpdf_get_pdf_version(pdf);
    return( ScalarString(ans ? mkChar(ans) : R_NaString) ); 
}

SEXP
R_qpdf_data_extension_level(SEXP r_pdf)
{
    qpdf_data pdf = (qpdf_data) R_ExternalPtrAddr(r_pdf);
    int ans = qpdf_get_pdf_extension_level(pdf);
    return( ScalarInteger(ans));
}
