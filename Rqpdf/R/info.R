
pdfInfo =
function(file, key)
{
    file = path.expand(file)
    if(!file.exists(file))
        stop("no such file ", file)
    
    .Call("R_get_pdf_info_val", file, as.character(key))
}

