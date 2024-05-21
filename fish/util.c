#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

#define BUFLEN 512

char* get_current_dir_name() {
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


void update_prompt() {
  char* cwd = get_current_dir_name();
  char* base = basename(cwd);
  printf("fish %s> ", base);
  free(cwd);
}


void print_process_status(pid_t pid, int status, int is_background) {
  char *buf = calloc(BUFLEN, sizeof(char));
  if (WIFEXITED(status)) {
    int exit_status = WEXITSTATUS(status);
    if (is_background) {
      snprintf(buf, BUFLEN, "        BG : %d exited, status=%d\n", pid, exit_status);
    } else {
      snprintf(buf, BUFLEN, "        FG : %d exited, status=%d\n", pid, exit_status);
    }
  } else if (WIFSIGNALED(status)) {
    int signal_number = WTERMSIG(status);
    if (is_background) {
      snprintf(buf, BUFLEN, "        BG : %d terminated by signal %d\n", pid, signal_number);
    } else {
      snprintf(buf, BUFLEN, "        FG : %d terminated by signal %d\n", pid, signal_number);
    }
  }
  write(STDERR_FILENO, buf, strlen(buf)*sizeof(char));
  free(buf);
}


void signal_handler(int signal) {
  if (signal == SIGCHLD) {
    // Nettoyer les processus zombies
    int status;
    int pid_wait;
    while ((pid_wait = waitpid(-1, &status, WNOHANG)) > 0) {
      if (WIFEXITED(status)) {
        print_process_status(pid_wait, status, 1);
      } else if (WIFSIGNALED(status)) {
        print_process_status(pid_wait, status, 1);
      }
    }
  }
}