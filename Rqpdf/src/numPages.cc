#include <qpdf/QPDF.hh>
#include <Rdefines.h>


extern "C"
SEXP
R_pdf_numPages(SEXP r_filename)
{    
	QPDF pdf;
	pdf.processFile(CHAR(STRING_ELT(r_filename, 0)));
	QPDFObjectHandle root = pdf.getRoot();
	QPDFObjectHandle pages = root.getKey("/Pages");
	QPDFObjectHandle count = pages.getKey("/Count");
        return(ScalarInteger(count.getIntValue()));
}
