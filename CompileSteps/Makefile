all: simpleMath.E sm sm2 libsimpleMath.a

%.E: %.c
	$(CC) -E -o $@ $(CPPFLAGS) $<

# You may need to compile the .o files with -fPIC flag for creating the .so
# PIC stands for Position Independent Code
%.o: %.c
	$(CC) -c -o $@ $(CPPFLAGS) $(CFLAGS) $<

libsimpleMath.so: simpleMath.o
	$(CC) -shared -o $@ simpleMath.o -lm

libsimpleMath.a: simpleMath.o
	$(AR) -cr $@ simpleMath.o 

sm: sm.o simpleMath.o
	$(CC) -o $@ simpleMath.o sm.o -lm

sm2: sm.o libsimpleMath.so
	$(CC) -o $@ sm.o -L. -lsimpleMath -lm

clean:
	-rm *.o *.a *.so sm sm2