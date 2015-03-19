CC=gcc
CFLAGS=-Wall -std=gnu11 -I.
LDFLAGS=-lm -lfftw3f
EXECUTABLES= x2kc2c t2wr2c mxyz2muvw
RFLAGS=-Ofast -march=native
DFLAGS=-g -O0 -march=native

all: release

release:
	$(foreach 	EXECUTABLE,$(EXECUTABLES),$(CC) $(CFLAGS) $(RFLAGS) $(EXECUTABLE).c core/*.c -o $(EXECUTABLE) $(LDFLAGS) &&) true

valgrind:
	$(foreach 	EXECUTABLE,$(EXECUTABLES),$(CC) $(CFLAGS) $(DFLAGS) $(EXECUTABLE).c core/*.c -o $(EXECUTABLE) $(LDFLAGS) &&) true

sanitizer:
	$(foreach 	EXECUTABLE,$(EXECUTABLES),$(CC) $(CFLAGS) $(DFLAGS) -fsanitize=address -fno-omit-frame-pointer $(EXECUTABLE).c core/*.c -o $(EXECUTABLE) $(LDFLAGS) &&) true

clean:
	$(foreach EXECUTABLE,$(EXECUTABLES),rm -rf $(EXECUTABLE) &&) true

.PHONY: all clean
