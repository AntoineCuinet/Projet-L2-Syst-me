#ifndef EXECUTE_COMMAND_PIPE_H
#define EXECUTE_COMMAND_PIPE_H

/**
 * @brief Execute a command line containing exactly one pipe.
 *
 * This function handles the execution of a command line that includes exactly one pipe
 * between two commands. It sets up the necessary pipe, forks two child processes to 
 * execute the commands, and manages the redirections of standard input and output 
 * accordingly. After forking, it waits for both child processes to complete and prints
 * their statuses.
 *
 * @param li A pointer to a `struct line` containing the parsed command line with exactly two commands.
 *
 * @return 0 on success, 1 on error with error messages printed to stderr.
 */
int execute_line_with_one_pipe(struct line *li);

/**
 * @brief Execute a command line containing multiple pipes.
 *
 * This function handles the execution of a command line that includes multiple pipes
 * between commands. It sets up the necessary pipes, forks child processes to execute 
 * each command, and manages the redirections of standard input and output accordingly.
 * After forking, it waits for all child processes to complete and prints their statuses.
 *
 * @param li A pointer to a `struct line` containing the parsed command line with multiple commands.
 *
 * @return 0 on success, 1 on error.
 */
int execute_line_with_pipes(struct line *li);

#endif /* EXECUTE_COMMAND_PIPE_H */