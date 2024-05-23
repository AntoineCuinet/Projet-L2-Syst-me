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


/**
 * @brief Get the current working directory.
 *
 * This function allocates memory and retrieves the current working directory.
 * The caller is responsible for freeing the allocated memory.
 *
 * @return char* The current working directory.
 */
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

/**
 * @brief Update the shell prompt with the current directory.
 *
 * This function updates the shell prompt to display the current working directory.
 */
void update_prompt() {
  char* cwd = get_current_dir_name();
  char* base = basename(cwd);
  printf("fish %s> ", base);
  free(cwd);
}

/**
 * @brief Remove an element from an array of pids.
 *
 * This function removes an element at the specified index from the array and shifts
 * the remaining elements to fill the gap.
 *
 * @param array The array of pids.
 * @param size The size of the array.
 * @param index The index of the element to remove.
 */
void remove_element(volatile pid_t *array, int size, size_t index) {
  for (int i = index; i < size - 1; ++i) {
    array[i] = array[i+1];
  }
  array[size - 1] = 0;
}

/**
 * @brief Remove terminated background processes from the array.
 *
 * This function checks the background processes array and removes any processes
 * that have terminated.
 */
void remove_terminated_bg_process() {
  size_t i = 0;
  while (i < MAX_CMDS) {
    if (bg_processes[i] == -1) {
      remove_element(bg_processes, MAX_CMDS, i);
      bg_index--;
    } else {
      i++;
    }
  }
}

/**
 * @brief Print the status of a process.
 *
 * This function prints the status of a foreground or background process, including
 * whether it exited normally or was terminated by a signal.
 *
 * @param pid The process ID.
 * @param status The status returned by waitpid.
 * @param is_background A flag indicating if the process is a background process.
 */
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

/**
 * @brief Signal handler for SIGCHLD.
 *
 * This function handles the SIGCHLD signal, which indicates that a child process
 * has terminated. It cleans up the terminated background processes.
 *
 * @param signal The signal number.
 */
void signal_handler(int signal) {
  if (signal == SIGCHLD) {
    int status;
    int pid_wait;

    // Clean up zombie processes
    for (size_t i = 0; i < bg_index; ++i){
      if (bg_processes[i] >= 1 && (pid_wait = waitpid(bg_processes[i], &status, WNOHANG)) > 0 ) {
        print_process_status(pid_wait, status, 1);
        bg_processes[i] = -1;
      }
    }
    remove_terminated_bg_process();
  }
}