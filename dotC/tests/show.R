dyn.load("dotC.so")
z = as.numeric(1:100)
o = .C("add1", z, length(z), ans = numeric(length(z))) $ans
o
