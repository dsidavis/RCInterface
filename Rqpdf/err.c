#include <qpdf/qpdf-c.h>
#include <Rdefines.h>

// Get the PDF version of the given file

SEXP
R_get_pdf_version(SEXP r_filename)
{
    qpdf_data tmp = qpdf_init();
    QPDF_ERROR_CODE err = qpdf_read(tmp, CHAR(STRING_ELT(r_filename, 0)));
    if(err != QPDF_SUCCESS) {
	PROBLEM "failed to read %s", CHAR(STRING_ELT(r_filename, 0))
	    ERROR;
    }
    
    char const * ans = qpdf_get_pdf_version(tmp);
    SEXP rans = ScalarString(mkChar(ans));
    qpdf_cleanup(tmp);
    return(rans);
}
