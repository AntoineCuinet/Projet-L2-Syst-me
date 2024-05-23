#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

/**
 * @brief Redirect the standard input to a file.
 *
 * This function redirects the standard input (stdin) to the specified file.
 *
 * @param filename The name of the file to use as the new standard input.
 * @return int Returns 0 on success, or 1 on failure.
 */
int redirect_input(char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    if (dup2(fd, STDIN_FILENO) == -1) {
        perror("dup2");
        close(fd);
        return 1;
    }

    if (close(fd) == -1) {
        perror("close");
        return 1;
    }
    return 0;
}


/**
 * @brief Redirect the standard output to a file (truncate mode).
 *
 * This function redirects the standard output (stdout) to the specified file,
 * truncating the file if it already exists or creating it if it does not.
 *
 * @param filename The name of the file to use as the new standard output.
 * @return int Returns 0 on success, or 1 on failure.
 */
int redirect_output_trunc(char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    if (dup2(fd, STDOUT_FILENO) == -1) {
        perror("dup2");
        close(fd);
        return 1;
    }

    if (close(fd) == -1) {
        perror("close");
        return 1;
    }
    return 0;
}


/**
 * @brief Redirect the standard output to a file (append mode).
 *
 * This function redirects the standard output (stdout) to the specified file,
 * appending to the file if it already exists or creating it if it does not.
 *
 * @param filename The name of the file to use as the new standard output.
 * @return int Returns 0 on success, or 1 on failure.
 */
int redirect_output_append(char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    if (dup2(fd, STDOUT_FILENO) == -1) {
        perror("dup2");
        close(fd);
        return 1;
    }

    if (close(fd) == -1) {
        perror("close");
        return 1;
    }
    return 0;
}