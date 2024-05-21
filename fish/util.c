#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "cmdline.h"

volatile pid_t bg_processes[MAX_CMDS];
volatile size_t bg_index = 0;

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


void remove_element(volatile pid_t *array, int size, size_t index) {
  for (int i = index; i < size - 1; ++i) {
    array[i] = array[i+1];
  }
  array[size - 1] = 0;
}


void remove_terminated_bg_process() {
  size_t i = 0;
  while (i < MAX_CMDS) {
    if (bg_processes[i] == -1) {
      remove_element(bg_processes, MAX_CMDS, i);
      bg_index--;
    }
    else
      i++;
  }
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

    for (size_t i = 0; i < bg_index; ++i){
      if (bg_processes[i] >= 1 && (pid_wait = waitpid(bg_processes[i], &status, WNOHANG)) > 0 ) {
        print_process_status(pid_wait, status, 1);
        bg_processes[i] = -1;
      }
    }
    remove_terminated_bg_process();
  }
}
