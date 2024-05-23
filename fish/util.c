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

volatile pid_t bg_processes[MAX_CMDS];
volatile size_t bg_index = 0;
volatile pid_t fg_processes[MAX_CMDS];
volatile size_t fg_index = 0;

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
 * @brief Remove a foreground process from the process list.
 *
 * This function searches for a given process ID in the list of foreground processes
 * and removes it if found. It shifts the remaining process IDs in the list to
 * fill the gap left by the removed process and decrements the foreground process index.
 *
 * @param pid_to_remove The process ID of the foreground process to remove.
 *
 */
void remove_fg_process(pid_t pid_to_remove) {
  for (size_t i = 0; i < MAX_CMDS; ++i) {
    if (fg_processes[i] == pid_to_remove) {
      remove_element(fg_processes, MAX_CMDS, i);
      fg_index--;
    }
  }
}

int is_input_redirected() {
    // Vérifier si l'entrée standard est redirigée
    return !isatty(STDIN_FILENO);
}

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
    snprintf(buf, BUFLEN, "    %s: %d exited, status=%d\n", is_background ? "BG" : "FG", pid, exit_status);
  } else if (WIFSIGNALED(status)) {
    int signal_number = WTERMSIG(status);
    snprintf(buf, BUFLEN, "    %s: %d terminated by signal %d\n", is_background ? "BG" : "FG", pid, signal_number);
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


/**
 * @brief Execute a command either in the foreground or background.
 *
 * This function handles the execution of an external command. It forks a child process
 * to run the command. If the command is to be run in the background, it ensures that
 * the input is redirected appropriately and does not block the shell. For foreground
 * commands, it waits for the command to complete and manages the foreground process
 * list.
 *
 * @param cmd The command to execute (e.g., "ls", "grep", etc.).
 * @param args The arguments for the command, including the command itself as args[0].
 * @param bg A flag indicating if the command should run in the background (non-zero) or foreground (zero).
 *
 * @return 0 on success, 1 on error with error messages printed to stderr.
 *
 * Example usage:
 * @code
 * char *args[] = {"ls", "-l", NULL};
 * execute_command("ls", args, 0); // Run 'ls -l' in the foreground.
 * @endcode
 */
int execute_command(char *cmd, char **args, int bg) {
  pid_t pid = fork();
  
  if (pid == -1) {
    perror("fork");
    return 1;
  }

  if (pid == 0) { // Processus enfant
    if (bg) {
      // Rediriger l'entrée standard vers /dev/null pour les processus en arrière-plan
      if (!is_input_redirected()) {
        if (redirect_input_to_dev_null() != 0) {
          return 1;
        }
      }
    } else {
      // Réinitialiser le gestionnaire de SIGINT à la valeur par défaut pour les commandes en avant-plan
      struct sigaction default_sigint;
      sigemptyset(&default_sigint.sa_mask);
      default_sigint.sa_flags = SA_RESTART;
      default_sigint.sa_handler = SIG_DFL;
      if (sigaction(SIGINT, &default_sigint, NULL) == -1) {
        perror("sigaction");
        return 1;
      }
    }
    
    execvp(cmd, args);
    char error_message[256];
    snprintf(error_message, 256, "Exec error: %s", cmd);
    perror(error_message);
    return 1;
  } else {
    if (bg) {
      bg_processes[bg_index++] = pid;
    } else {
      fg_processes[fg_index++] = pid;
      while (fg_index > 0) {
        int status;
        pid_t res = wait(&status);
        if (res == -1) {
          perror("wait");
          return 1;
        } else {
          print_process_status(res, status, bg);
          remove_fg_process(res);
        }
      }
    }
  }
  return 0;
}
