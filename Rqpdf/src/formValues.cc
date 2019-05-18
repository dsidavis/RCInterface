#include "Rqpdf.h"

SEXP FieldObjHelperToR(QPDFFormFieldObjectHelper ffh);

extern "C"
SEXP
R_getFormValues(SEXP r_filename)
{
	QPDF qpdf;
	qpdf.processFile(CHAR(STRING_ELT(r_filename, 0)));

        QPDFAcroFormDocumentHelper afdh(qpdf);
        QPDFPageDocumentHelper pdh(qpdf);
        std::vector<QPDFPageObjectHelper> pages = pdh.getAllPages();

        SEXP ans;
        PROTECT(ans = NEW_LIST(pages.size()));

        int page_ctr = 0;
        for (std::vector<QPDFPageObjectHelper>::iterator page_iter =
                 pages.begin();
             page_iter != pages.end(); ++page_iter, page_ctr++)
        {
            // Get all widget annotations for each page. Widget
            // annotations are the ones that contain the details of
            // what's in a form field.
            std::vector<QPDFAnnotationObjectHelper> annotations = afdh.getWidgetAnnotationsForPage(*page_iter);

            SEXP el, names;
            el = NEW_LIST(annotations.size());
            SET_VECTOR_ELT(ans, page_ctr, el);
            PROTECT(names = NEW_CHARACTER(annotations.size()));
            int annotCtr = 0;
            
            for (std::vector<QPDFAnnotationObjectHelper>::iterator annot_iter = annotations.begin();
                      annot_iter != annotations.end(); ++annot_iter, annotCtr++)
            {
                QPDFFormFieldObjectHelper ffh = afdh.getFieldForAnnotation(*annot_iter);
                if(ffh.getFieldType() == "/Ch") {
                    Rprintf("got a /Ch.");
                    Rf_PrintValue(QPDFObjectHandleToR(annot_iter->getObjectHandle().getKey("/Opt")));
                }                
                SET_STRING_ELT(names, annotCtr, mkChar(ffh.getMappingName().c_str()));
                SET_VECTOR_ELT(el, annotCtr, FieldObjHelperToR(ffh));
            }
            SET_NAMES(el, names);
            UNPROTECT(1);
        }

        UNPROTECT(1);
        return(ans);
}

SEXP
convertQPDFArrayToR(QPDFObjectHandle h)
{
    int len = h.getArrayNItems();
    SEXP ans;
    PROTECT(ans = NEW_LIST(len));
    for(int i = 0; i < len; i++) 
        SET_VECTOR_ELT(ans, i, QPDFObjectHandleToR(h.getArrayItem(i)));

    UNPROTECT(1);
    return(ans);
}

SEXP
convertQPDFDictToR(QPDFObjectHandle h, bool followGen, bool stripSlash)
{
    std::set<std::string> keys = h.getKeys();
    int len = keys.size(), i = 0;
    SEXP ans, names;
    PROTECT(ans = NEW_LIST(len));
    PROTECT(names = NEW_CHARACTER(len));
    std::set<std::string>::iterator it = keys.begin();
    for( ; i < len; i++, ++it) {
        SET_VECTOR_ELT(ans, i, QPDFObjectHandleToR(h.getKey(*it)));
        SET_STRING_ELT(names, i, mkChar(it->c_str() + 1));
    }
    SET_NAMES(ans, names);
    UNPROTECT(2);
    return(ans);
}

SEXP
mkPDFIdGenRobj(QPDFObjectHandle h)
{
    SEXP ans;
#if 1
            PROTECT(ans = NEW_INTEGER(2));
            INTEGER(ans)[0] = h.getObjectID();
            INTEGER(ans)[1] = h.getGeneration();
#else
            PROTECT(ans = NEW_CHARACTER(1));
            char buf[100];
            sprintf(buf, "%d.%d",  h.getObjectID(), h.getGeneration());
            SET_STRING_ELT(ans, 0, mkChar(buf));
#endif            
            SET_CLASS(ans, ScalarString(mkChar("QPDFReference")));
            UNPROTECT(1);
            return(ans);
}

SEXP
QPDFObjectHandleToR(QPDFObjectHandle h, bool followGen, bool stripSlash)
{
    bool isOID = (h.getObjectID() > 0); 

    SEXP ans = R_NilValue;
    switch(h.getTypeCode()) {

    case QPDFObject::ot_boolean:
        ans = ScalarLogical(h.getBoolValue());
        break;
    case QPDFObject::ot_integer:
        ans = ScalarInteger(h.getIntValue());
        break;
    case QPDFObject::ot_real:
        ans = ScalarReal(h.getNumericValue());
        break;
    case QPDFObject::ot_string:
        ans = ScalarString(mkChar(h.getStringValue().c_str()));
        break;
    case QPDFObject::ot_name:
        ans = ScalarString(mkChar(h.getName().c_str()));
        break;
    case QPDFObject::ot_array:
        ans = convertQPDFArrayToR(h);
        break;
    case QPDFObject::ot_dictionary:
        if(!followGen && isOID)   // do we want to do this generally for all types of objects.
            ans = mkPDFIdGenRobj(h);
        else
            ans = convertQPDFDictToR(h, followGen, stripSlash);
        break;                                                                   
    default:
        ans = R_NilValue;            
        break;            
    }

    return(ans);
}


SEXP
FieldObjHelperToR(QPDFFormFieldObjectHelper ffh)
{

    // isNull, getFieldType(), getMappingName(), getValue(), getDefaultValue()
    SEXP ans, names;
    int numFields = 5, field = 0;
    PROTECT(ans = NEW_LIST(numFields));
    PROTECT(names = NEW_CHARACTER(numFields));

// Doesn't seem useful.
//    SET_VECTOR_ELT(ans, field, ScalarLogical(ffh.isNull()));
//    SET_STRING_ELT(names, field++, mkChar("isNull"));

#if 0    
    if(ffh.getFieldType() == "/Ch") {
        Rprintf("got a /Ch.  /Opt = '%s'\n",    ffh.getInheritableFieldValueAsString("/V").c_str());
    }
#endif
    
    SET_VECTOR_ELT(ans, field, ScalarString(mkChar(ffh.getFieldType().c_str())));
    SET_STRING_ELT(names, field++, mkChar("fieldType"));

    SET_VECTOR_ELT(ans, field, ScalarString(mkChar(ffh.getMappingName().c_str())));
    SET_STRING_ELT(names, field++, mkChar("name"));

    SET_VECTOR_ELT(ans, field, QPDFObjectHandleToR(ffh.getValue()));
    SET_STRING_ELT(names, field++, mkChar("value"));

    SET_VECTOR_ELT(ans, field, QPDFObjectHandleToR(ffh.getDefaultValue()));
    SET_STRING_ELT(names, field++, mkChar("defaultValue"));    

    SET_VECTOR_ELT(ans, field, ScalarString(mkChar(ffh.getValue().getTypeName())));
    SET_STRING_ELT(names, field++, mkChar("valueType"));                    


    SET_NAMES(ans, names);
    UNPROTECT(2);
    return(ans);
}
