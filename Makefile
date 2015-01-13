CC=gcc
CFLAGS=-Wall -std=gnu11 -I.
LDFLAGS=-lm -lfftw3f
EXECUTABLES= x2kc2c t2wr2c

all: release

release:
	$(foreach 	EXECUTABLE,$(EXECUTABLES),$(CC) $(CFLAGS) $(LDFLAGS) -Ofast -march=native $(EXECUTABLE).c core/*.c -o $(EXECUTABLE) &&) true

valgrind:
	$(foreach 	EXECUTABLE,$(EXECUTABLES),$(CC) $(CFLAGS) $(LDFLAGS) -g -O0 -march=native $(EXECUTABLE).c core/*.c -o $(EXECUTABLE) &&) true

sanitizer:
	$(foreach 	EXECUTABLE,$(EXECUTABLES),$(CC) $(CFLAGS) $(LDFLAGS) -g -O0 -fsanitize=address -fno-omit-frame-pointer $(EXECUTABLE).c core/*.c -o $(EXECUTABLE) &&) true

clean:
	$(foreach EXECUTABLE,$(EXECUTABLES),rm -rf $(EXECUTABLE) &&) true

.PHONY: all clean
