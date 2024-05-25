#ifndef EXECUTE_CMD_H
#define EXECUTE_CMD_H


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
 * @brief Execute a command either in the foreground or background, with or without pipes.
 *
 * This function handles the execution of internal commands like `cd` and `exit`, as well as 
 * external commands, both with and without pipes. It supports running commands in the 
 * background or foreground, and handles input redirection for background processes.
 * If the command line contains multiple commands separated by pipes, it delegates to
 * functions designed to handle one or multiple pipes.
 *
 * @param cmd The command to execute (e.g., "ls", "grep", etc.).
 * @param args The arguments for the command, including the command itself as args[0].
 * @param bg A flag indicating if the command should run in the background (non-zero) or foreground (zero).
 * @param li A pointer to the `struct line` containing the parsed command line, including commands, 
 *           arguments, input/output file redirections, and background execution flag.
 *
 * @return 0 on success, 1 on error with error messages printed to stderr.
 */
int execute_command(char *cmd, char **args, int bg, struct line *li);


#endif /* EXECUTE_CMD_H */