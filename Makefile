CC=gcc
CFLAGS= -std=gnu11 -I.
LDFLAGS= $(RFLAGS) -L. -lfftw3f_omp -lfftw3f -lm -lgsl -fopenmp
EXECUTABLES= x2kc2c t2wr2c mxyz2muvw avg
RFLAGS= -O3 -march=native -pipe -flto -funroll-loops
DFLAGS= -g -Og

all: release

release:
	$(foreach       EXECUTABLE,$(EXECUTABLES),$(CC) $(CFLAGS) $(RFLAGS) $(EXECUTABLE).c core/*.c -o $(EXECUTABLE) $(LDFLAGS) &&) true

valgrind:
	$(foreach       EXECUTABLE,$(EXECUTABLES),$(CC) $(CFLAGS) $(DFLAGS) $(EXECUTABLE).c core/*.c -o $(EXECUTABLE) $(LDFLAGS) &&) true

sanitizer:
	$(foreach       EXECUTABLE,$(EXECUTABLES),$(CC) $(CFLAGS) $(DFLAGS) -fsanitize=address -fno-omit-frame-pointer $(EXECUTABLE).c core/*.c -o $(EXECUTABLE) $(LDFLAGS) &&) true

clean:
	$(foreach EXECUTABLE,$(EXECUTABLES),rm -rf $(EXECUTABLE) &&) true

.PHONY: all clean

install:
	$(foreach EXECUTABLE,$(EXECUTABLES),ln -sf $(shell pwd)/$(EXECUTABLE) /usr/local/bin/smarg-$(EXECUTABLE) &&) true

uninstall:
	$(foreach EXECUTABLE,$(EXECUTABLES),ln -rf $(shell pwd) /usr/local/bin/smarg-$(EXECUTABLE)) &&) true
