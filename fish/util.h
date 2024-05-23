#ifndef UTIL_H
#define UTIL_H

// #include <sys/types.h>
// #include "cmdline.h"

// extern volatile pid_t bg_processes[MAX_CMDS];
// extern volatile size_t bg_index;
// extern volatile pid_t fg_processes[MAX_CMDS];
// extern volatile size_t fg_index;


/**
 * @brief Get the current working directory.
 *
 * This function allocates memory and retrieves the current working directory.
 * The caller is responsible for freeing the allocated memory.
 *
 * @return char* The current working directory.
 */
char* get_current_dir_name();

/**
 * @brief Update the shell prompt with the current directory.
 *
 * This function updates the shell prompt to display the current working directory.
 */
void update_prompt();

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
void remove_element(volatile pid_t *array, int size, size_t index);

/**
 * @brief Remove terminated background processes from the array.
 *
 * This function checks the background processes array and removes any processes
 * that have terminated.
 */
void remove_terminated_bg_process();

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
void remove_fg_process(pid_t pid_to_remove);


int is_input_redirected();
int redirect_input_to_dev_null();




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
void print_process_status(pid_t pid, int status, int is_background);

/**
 * @brief Signal handler for SIGCHLD.
 *
 * This function handles the SIGCHLD signal, which indicates that a child process
 * has terminated. It cleans up the terminated background processes.
 *
 * @param signal The signal number.
 */
void signal_handler(int signal);


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
int execute_command(char *cmd, char **args, int bg);

#endif /* UTIL_H */