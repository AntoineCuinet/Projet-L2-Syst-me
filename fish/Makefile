# CUINET Antoine - Makefile - fish

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -g -I. -Iextern_cmd -Iintern_cmd -fsanitize=address
LDFLAGS = -g -L. -fsanitize=address
LDLIBS = -lcmdline

all: libcmdline.so libutil.so fish cmdline_test

libcmdline.so: cmdline.o
	$(CC) $(LDFLAGS) -shared -o $@ $^

libutil.so: util.o 
	$(CC) $(LDFLAGS) -shared -o $@ $^

fish: fish.o extern_cmd/extern_cmd.o intern_cmd/intern_cmd.o redirect_cmd/redirect_cmd.o background_cmd/background_cmd.o libcmdline.so libutil.so
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

cmdline_test: cmdline_test.o libcmdline.so
	$(CC) $(LDFLAGS) $< -o $@ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

extern_cmd/extern_cmd.o: extern_cmd/extern_cmd.c extern_cmd/extern_cmd.h
	$(CC) $(CFLAGS) -c $< -o $@

intern_cmd/intern_cmd.o: intern_cmd/intern_cmd.c intern_cmd/intern_cmd.h
	$(CC) $(CFLAGS) -c $< -o $@

redirect_cmd/redirect_cmd.o: redirect_cmd/redirect_cmd.c redirect_cmd/redirect_cmd.h
	$(CC) $(CFLAGS) -c $< -o $@

background_cmd/background_cmd.o: background_cmd/background_cmd.c background_cmd/background_cmd.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f extern_cmd/*.o
	rm -f intern_cmd/*.o

mrproper: clean
	rm -f libcmdline.so libutil.so fish cmdline_test
