#ifndef UTIL_H
#define UTIL_H


extern volatile pid_t bg_processes[MAX_CMDS];
extern volatile size_t bg_index;
extern volatile pid_t fg_processes[MAX_CMDS];
extern volatile size_t fg_index;


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
 * @brief Checks if the standard input (stdin) is redirected.
 * 
 * This function checks whether the standard input is coming from a terminal
 * (tty) or if it has been redirected from a file or another source. It uses
 * the `isatty` function to determine this.
 * 
 * @return int Returns 0 if stdin is coming from a terminal (not redirected),
 *             and returns a non-zero value if stdin is redirected.
 */
int is_input_redirected();

/**
 * @brief Redirects the standard input (stdin) to /dev/null.
 * 
 * This function opens /dev/null for reading and redirects the standard input
 * to read from /dev/null instead. This is useful for background processes that
 * should not read from the terminal.
 * 
 * @return int Returns 0 on success, or 1 if an error occurs.
 */
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

#endif /* UTIL_H */