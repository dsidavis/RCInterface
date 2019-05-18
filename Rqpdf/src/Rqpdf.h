#ifndef RQPDF_H
#define RQPDF_H 1

#include <qpdf/QPDF.hh>
#include <qpdf/QPDFPageDocumentHelper.hh>
#include <qpdf/QPDFAcroFormDocumentHelper.hh>


#include <Rdefines.h>
#undef isNull 

SEXP QPDFObjectHandleToR(QPDFObjectHandle h, bool followGen = false, bool stripSlash = true);
SEXP convertQPDFDictToR(QPDFObjectHandle h, bool followGen = false, bool stripSlash = true);
SEXP R_qpdf_dictKeys(QPDFObjectHandle h);

#endif
