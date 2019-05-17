rusage =
function(who = 0L)
{
    ans = .Call("R_getrusage", as.integer(who))
    names(ans) = c("maxrss", "ixrss", "idrss", "isrss", "minflt", "majflt", "nswap", "inblock", "oublock", "msgsnd", "msgrcv", "nsignals", "nvcsw", "nivcsw")
    ans
}


rusage2 =
function(who = 0L)
{
    ans = .Call("R_getrusage2", as.integer(who))
    names(ans) = c("utime", "stime", "maxrss", "ixrss", "idrss", "isrss", "minflt", "majflt", "nswap", "inblock", "oublock", "msgsnd", "msgrcv", "nsignals", "nvcsw", "nivcsw")
    ans
}
