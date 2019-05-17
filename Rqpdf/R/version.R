libqpdfVersion = "8.2.1"


version =
function(runTime = TRUE)    
{
    if(runTime)
        .Call("R_qpdf_version_rt")
    else
        libqpdfVersion
}
