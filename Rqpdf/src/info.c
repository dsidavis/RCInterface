#include <qpdf/qpdf-c.h>
#include <Rdefines.h>

// Get the key from the Info dictionary

SEXP
R_get_pdf_info_val(SEXP r_filename, SEXP r_key)
{
    qpdf_data tmp = qpdf_init();
    QPDF_ERROR_CODE err = qpdf_read(tmp, CHAR(STRING_ELT(r_filename, 0)), NULL);
    if(err != QPDF_SUCCESS) {
	PROBLEM "failed to read %s", CHAR(STRING_ELT(r_filename, 0))
	    ERROR;
    }
    
    char const * ans = qpdf_get_info_key(tmp, CHAR(STRING_ELT(r_key, 0)));
    SEXP rans = ScalarString(ans ? mkChar(ans) : R_NaString);
    qpdf_cleanup(&tmp);
    return(rans);
}
