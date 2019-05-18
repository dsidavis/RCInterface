#include <qpdf/QPDF.hh>
#include <qpdf/QPDFPageDocumentHelper.hh>
#include <qpdf/QPDFAcroFormDocumentHelper.hh>

#include <Rdefines.h>

extern SEXP QPDFObjectHandleToR(QPDFObjectHandle h, bool followGen = false);
SEXP R_qpdf_dictKeys(QPDFObjectHandle h);

extern "C"
SEXP
R_getRoot(SEXP r_filename)
{
   QPDF qpdf;
   qpdf.processFile(CHAR(STRING_ELT(r_filename, 0)));
   QPDFObjectHandle r = qpdf.getRoot();
   return(QPDFObjectHandleToR(r, true));
   // return(R_qpdf_dictKeys(r)); 
}

extern "C"
SEXP
R_getTrailer(SEXP r_filename)
{
   QPDF qpdf;
   qpdf.processFile(CHAR(STRING_ELT(r_filename, 0)));
   QPDFObjectHandle r = qpdf.getTrailer();
//   return(R_qpdf_dictKeys(r));
   return(QPDFObjectHandleToR(r, false));
}

void
R_freeQPDF(SEXP v)
{
    QPDF *q = (QPDF *) R_ExternalPtrAddr(v);
    if(q)
        delete q;
}

SEXP
R_getQPDF(SEXP r_filename)
{
    QPDF *qpdf;
    qpdf = new QPDF();
    if(Rf_length(r_filename))
        qpdf->processFile(CHAR(STRING_ELT(r_filename, 0)));

    SEXP ans;
    ans = R_MakeExternalPtr(qpdf, Rf_install("QPDF"), R_NilValue);
    PROTECT(ans);
    R_RegisterCFinalizer(ans, R_freeQPDF);
    UNPROTECT(1);
    return(ans);
}



extern "C"
SEXP
R_QPDFObjectHandle_getKeys(SEXP r_obj)
{
    QPDFObjectHandle *p = (QPDFObjectHandle *) R_ExternalPtrAddr(r_obj);
    if(p->getTypeCode() != QPDFObject::ot_dictionary) {
        PROBLEM "QPDF reference is not a dictionary, but a %s", p->getTypeName()
            ERROR;
    }
    return(R_qpdf_dictKeys(*p));
}


SEXP
R_qpdf_dictKeys(QPDFObjectHandle h)
{
    std::set<std::string> keys = h.getKeys();
    int len = keys.size(), i = 0;
    SEXP names;
    PROTECT(names = NEW_CHARACTER(len));
    std::set<std::string>::iterator it = keys.begin();
    for( ; i < len; i++, ++it) 
        SET_STRING_ELT(names, i, mkChar(it->c_str()));
    
    UNPROTECT(1);
    return(names);
}
