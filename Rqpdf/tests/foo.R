
tmp = .Call("R_qpdf_data_init")
.Call("R_qpdf_data_read", tmp, "inst/sampleDocs/rplot.pdf")
.Call("R_qpdf_data_info", tmp, "CreationDate")
.Call("R_qpdf_data_info", tmp, "Creator")

