#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include "cmdline.h"

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
  fflush(stdout);  // Force the output buffer to be flushed
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
 * @brief Checks if the standard input (stdin) is redirected.
 * 
 * This function checks whether the standard input is coming from a terminal
 * (tty) or if it has been redirected from a file or another source. It uses
 * the `isatty` function to determine this.
 * 
 * @return int Returns 0 if stdin is coming from a terminal (not redirected),
 *             and returns a non-zero value if stdin is redirected.
 */
int is_input_redirected() {
    // Check if standard input is redirected
    return !isatty(STDIN_FILENO);
}

/**
 * @brief Redirects the standard input (stdin) to /dev/null.
 * 
 * This function opens /dev/null for reading and redirects the standard input
 * to read from /dev/null instead. This is useful for background processes that
 * should not read from the terminal.
 * 
 * @return int Returns 0 on success, or 1 if an error occurs.
 */
int redirect_input_to_dev_null() {
    int dev_null_fd = open("/dev/null", O_RDONLY);
    if (dev_null_fd == -1) {
        perror("Error opening /dev/null");
        return 1;
    }

    if (dup2(dev_null_fd, STDIN_FILENO) == -1) {
        perror("Error redirecting input to /dev/null");
        close(dev_null_fd);
        return 1;
    }

    if (close(dev_null_fd) == -1) {
        perror("close");
        return 1;
    }
    return 0;
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
    snprintf(buf, BUFLEN, "%s: %d exited, status=%d\n", is_background ? "\n\tBG" : "\tFG", pid, exit_status);
  } else if (WIFSIGNALED(status)) {
    int signal_number = WTERMSIG(status);
    snprintf(buf, BUFLEN, "%s: %d terminated by signal %d\n", is_background ? "\n\tBG" : "\tFG", pid, signal_number);
  }
  write(STDERR_FILENO, buf, strlen(buf)*sizeof(char));
  free(buf);
  if (is_background) {
    update_prompt();
  }
}
