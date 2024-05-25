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

fish: fish.o intern_cmd/intern_cmd.o redirect_cmd/redirect_cmd.o execute_cmd/execute_cmd.o pipe_cmd/pipe_cmd.o libcmdline.so libutil.so
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

cmdline_test: cmdline_test.o libcmdline.so
	$(CC) $(LDFLAGS) $< -o $@ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

intern_cmd/intern_cmd.o: intern_cmd/intern_cmd.c intern_cmd/intern_cmd.h
	$(CC) $(CFLAGS) -c $< -o $@

redirect_cmd/redirect_cmd.o: redirect_cmd/redirect_cmd.c redirect_cmd/redirect_cmd.h
	$(CC) $(CFLAGS) -c $< -o $@

execute_cmd/execute_cmd.o: execute_cmd/execute_cmd.c execute_cmd/execute_cmd.h
	$(CC) $(CFLAGS) -c $< -o $@

pipe_cmd/pipe_cmd.o: pipe_cmd/pipe_cmd.c pipe_cmd/pipe_cmd.h
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f *.o
	rm -f intern_cmd/*.o
	rm -f redirect_cmd/*.o
	rm -f pipe_cmd/*.o

mrproper: clean
	rm -f libcmdline.so libutil.so fish cmdline_test
