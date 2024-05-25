#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "cmdline.h"
#include "util.h"

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
int execute_line_with_one_pipes(struct line *li) {
    if (li->n_cmds != 2) {
        fprintf(stderr, "This function supports exactly one pipe between two commands.\n");
        return 1;
    }

    int pipefd[2];
    pid_t pids[2];

    // Create the pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    // Fork the first child for the first command
    pids[0] = fork();
    if (pids[0] == -1) {
        perror("fork");
        return 1;
    }

    if (pids[0] == 0) { // In the first child process
        
        // Redirect stdout to the pipe
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            return 2;
        }

        // Close unused pipe ends
        close(pipefd[0]);
        close(pipefd[1]);

        // Execute the first command
        execvp(li->cmds[0].args[0], li->cmds[0].args);
        perror("execvp");
        return 2;
    }

    // Fork the second child for the second command
    pids[1] = fork();
    if (pids[1] == -1) {
        perror("fork");
        return 1;
    }

    if (pids[1] == 0) { // In the second child process

        // Redirect stdin to the pipe
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        // Close unused pipe ends
        close(pipefd[0]);
        close(pipefd[1]);

        // Execute the second command
        execvp(li->cmds[1].args[0], li->cmds[1].args);
        perror("execvp");
        exit(EXIT_FAILURE);
    }

    // Parent process

    // Close unused pipe ends
    close(pipefd[0]);
    close(pipefd[1]);

    // Wait for both child processes to finish and print their status
    for (int i = 0; i < 2; i++) {
        int status;
        pid_t res = waitpid(pids[i], &status, 0);
        if (res == -1) {
            perror("waitpid");
            return 1;
        }
        print_process_status(res, status, li->background);
    }

    return 0;
}



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
int execute_line_with_pipes(struct line *li) {
    int pipefd[2 * (li->n_cmds - 1)];
    pid_t pids[li->n_cmds];

    // Create the necessary tubes
    for (size_t i = 0; i < li->n_cmds - 1; i++) {
        if (pipe(pipefd + i * 2) == -1) {
            perror("pipe");
            return 1;
        }
    }

    for (size_t i = 0; i < li->n_cmds; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork");
            return 1;
        }

        if (pids[i] == 0) { // Child processes

            // Redirect input
            if (i > 0) {
                if (dup2(pipefd[(i - 1) * 2], STDIN_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            // Redirect output
            if (i < li->n_cmds - 1) {
                if (dup2(pipefd[i * 2 + 1], STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            // Close all pipe descriptors in child
            for (size_t j = 0; j < 2 * (li->n_cmds - 1); j++) {
                close(pipefd[j]);
            }

            // Execute the command
            execvp(li->cmds[i].args[0], li->cmds[i].args);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }

    // Close all pipe descriptors in parent
    for (size_t i = 0; i < 2 * (li->n_cmds - 1); i++) {
        close(pipefd[i]);
    }

    // Wait for all child processes
    for (size_t i = 0; i < li->n_cmds; i++) {
        int status;
        waitpid(pids[i], &status, 0);
    }

    return 0;
}
