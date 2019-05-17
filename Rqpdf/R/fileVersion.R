
pdfVersion =
function(file)
{
    file = path.expand(file)
    if(!file.exists(file))
        stop("no such file ", file)
    
    .Call("R_get_pdf_version", file)
}

