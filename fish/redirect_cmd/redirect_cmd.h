#ifndef REDIRECT_COMMAND_H
#define REDIRECT_COMMAND_H

/**
 * @brief Redirect the standard input to a file.
 *
 * This function redirects the standard input (stdin) to the specified file.
 *
 * @param filename The name of the file to use as the new standard input.
 * @return int Returns 0 on success, or 1 on failure.
 */
int redirect_input(char *filename);

/**
 * @brief Redirect the standard output to a file (truncate mode).
 *
 * This function redirects the standard output (stdout) to the specified file,
 * truncating the file if it already exists or creating it if it does not.
 *
 * @param filename The name of the file to use as the new standard output.
 * @return int Returns 0 on success, or 1 on failure.
 */
int redirect_output_trunc(char *filename);

/**
 * @brief Redirect the standard output to a file (append mode).
 *
 * This function redirects the standard output (stdout) to the specified file,
 * appending to the file if it already exists or creating it if it does not.
 *
 * @param filename The name of the file to use as the new standard output.
 * @return int Returns 0 on success, or 1 on failure.
 */
int redirect_output_append(char *filename);

#endif /* REDIRECT_COMMAND_H */