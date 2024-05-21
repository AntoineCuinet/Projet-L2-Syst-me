#ifndef EXECUTE_COMMAND_EXTERN_H
#define EXECUTE_COMMAND_EXTERN_H



void remove_fg_process(pid_t pid_to_remove);

/**
 * @brief Execute a command with arguments.
 * 
 * This function creates a new process with fork(), then in the child process,
 * executes the specified command with the given arguments using execvp().
 * It then waits for the child process to finish with waitpid().
 * 
 * @param cmd The name of the command to execute.
 * @param args An array of strings representing the arguments of the command.
 *             The last element of the array must be NULL to indicate the end of arguments.
 * @return 0 if the command executed successfully, 1 on error.
 */
int execute_command_extern(char *cmd, char **args, pid_t pid, int bg);

#endif /* EXECUTE_COMMAND_EXTERN_H */