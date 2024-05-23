#ifndef EXECUTE_COMMAND_INTERN_H
#define EXECUTE_COMMAND_INTERN_H

/**
 * @brief Change the current working directory.
 *
 * This function implements the 'cd' command for the shell. It handles various forms
 * of the 'cd' command, including 'cd', 'cd ~', 'cd ~user', 'cd ~user/path', and 'cd path'.
 *
 * @param args Array of arguments where args[0] is "cd" and args[1] is the target directory.
 * @return int Returns 0 on success, or 1 on failure.
 */
int execute_command_intern_cd(char **args);

/**
 * @brief Exit the shell.
 *
 * This function implements the 'exit' command for the shell. It terminates the shell
 * with the specified exit status or with EXIT_SUCCESS if no status is provided.
 *
 * @param li Pointer to the line structure.
 * @param cmd Pointer to the command structure.
 * @return int This function does not return; it exits the program.
 */
int execute_command_intern_exit(struct line *li, struct cmd *cmd);

#endif /* EXECUTE_COMMAND_INTERN_H */