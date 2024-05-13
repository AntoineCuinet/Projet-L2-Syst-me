#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <signal.h>
#include <sys/wait.h>

char* get_current_dir_name(int BUFLEN) {
  char* cwd = (char*)malloc(BUFLEN * sizeof(char));
  if (cwd == NULL) {
    perror("fish");
    exit(EXIT_FAILURE);
  }

  if (getcwd(cwd, BUFLEN) == NULL) {
    perror("fish");
    free(cwd);
    exit(EXIT_FAILURE);
  }

  return cwd;
}


void update_prompt(int BUFLEN) {
  char* cwd = get_current_dir_name(BUFLEN);
  char* base = basename(cwd);
  printf("fish %s> ", base);
  free(cwd);
}


void print_process_status(pid_t pid, int status, int is_background) {
  if (WIFEXITED(status)) {
    int exit_status = WEXITSTATUS(status);
    if (is_background) {
      fprintf(stderr, "        BG : %d exited, status=%d\n", pid, exit_status);
    } else {
      fprintf(stderr, "        FG : %d exited, status=%d\n", pid, exit_status);
    }
  } else if (WIFSIGNALED(status)) {
    int signal_number = WTERMSIG(status);
    if (is_background) {
      fprintf(stderr, "        BG : %d terminated by signal %d\n", pid, signal_number);
    } else {
      fprintf(stderr, "        FG : %d terminated by signal %d\n", pid, signal_number);
    }
  }
}
