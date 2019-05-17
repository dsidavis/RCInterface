#include <qpdf/qpdf-c.h>
#include <Rdefines.h>

SEXP
R_qpdf_version_rt()
{
    char const *ans;
    ans = qpdf_get_qpdf_version();
    return(ScalarString(mkChar(ans)));
}
