
pdfInfo =
function(file, key)
{
    file = path.expand(file)
    if(!file.exists(file))
        stop("no such file ", file)

    # keys start with a / and if we omit this we get NA back.
    # So we add it for the caller if it is not there.
    if(!grepl("^/", key))
        key = paste0("/", key)
    
    .Call("R_get_pdf_info_val", file, as.character(key))
}

