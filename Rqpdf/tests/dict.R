library(Rqpdf)
qpdf = .Call("R_getQPDF", "tmp.pdf")
rr = .Call("R_getRoot", "tmp.pdf")

.Call("R_getObjectByID", qpdf, rr[[1]])


#
doc = qpdf("tmp.pdf")
rr = getRoot(doc)

# Resolve the reference to 3 0
a = doc[[ rr[[ 1 ]] ]]

# And to 4 0
b = doc[[ rr[[ 2 ]] ]]



