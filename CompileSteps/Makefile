
%.E: %.c
	$(CC) -E -o $@ $(CPPFLAGS) $<

%.o: %.c
	$(CC) -c -o $@ $(CPPFLAGS) $(CFLAGS) $<


sm: sm.o simpleMath.o
	$(CC) -o $@ simpleMath.o sm.o -lm
