getFormValues =
    #
    # Get all the widget annotations in the PDF
    # getting their type, name, value, type of the value, default value and page number
    # and whether the widget is NULL or not in the PDF sense.
    #
    #  This can return each widget as a separate element for each page
    # or collapse each page to a data.frame, or collapse all widgets across all pages
    # into a data frame.
function(pdf, asDataFrame = TRUE, combinePages = TRUE)
{
    pdf = path.expand(pdf)
    if(!file.exists(pdf))
        stop(pdf, " doesn't exist")

    z = .Call("R_getFormValues", pdf)
    if(!asDataFrame)
        return(z)

    if(combinePages) {
        formValuesToDF(unlist(z, recursive = FALSE), rep(seq(along = z), sapply(z, length)))
    } else
        mapply(formValuesToDF, z, seq(along = z), SIMPLIFY = FALSE)
}

formValuesToDF =
function(z, page = NA)
{    
    vars = names(z[[1]])
    tmp = lapply(vars, function(id) sapply(z, `[[`, id))
    names(tmp) = vars
    i = match("defaultValue", vars)
    d = as.data.frame(tmp[-i], stringsAsFactors = FALSE)
    d$defaultValue = I(tmp[[i]])
    d$page = page
    d
}
