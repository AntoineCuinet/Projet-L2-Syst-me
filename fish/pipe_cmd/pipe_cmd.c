#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "cmdline.h"
#include "util.h"
#include "execute_cmd/execute_cmd.h"

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
int execute_line_with_one_pipe(struct line *li) {
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
        if (li->background) {
            // Redirect standard input to /dev/null for background processes
            if (!is_input_redirected()) {
                if (redirect_input_to_dev_null() != 0) {
                    return 1;
                }
            }
        } else {
            // Reset SIGINT handler to default for foreground commands
            struct sigaction default_sigint;
            sigemptyset(&default_sigint.sa_mask);
            default_sigint.sa_flags = SA_RESTART;
            default_sigint.sa_handler = SIG_DFL;
            if (sigaction(SIGINT, &default_sigint, NULL) == -1) {
                perror("sigaction");
                return 1;
            }
        }
        

        // Redirect stdout to the pipe
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            return 2;
        }

        // Close unused pipe ends
        if (close(pipefd[0]) == -1 || close(pipefd[1]) == -1) {
            perror("close");
            return 1;
        }

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
        if (li->background) {
            // Redirect standard input to /dev/null for background processes
            if (!is_input_redirected()) {
                if (redirect_input_to_dev_null() != 0) {
                    return 1;
                }
            }
        } else {
            // Reset SIGINT handler to default for foreground commands
            struct sigaction default_sigint;
            sigemptyset(&default_sigint.sa_mask);
            default_sigint.sa_flags = SA_RESTART;
            default_sigint.sa_handler = SIG_DFL;
            if (sigaction(SIGINT, &default_sigint, NULL) == -1) {
                perror("sigaction");
                return 1;
            }
        }

        // Redirect stdin to the pipe
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            return 1;
        }

        // Close unused pipe ends
        if (close(pipefd[0]) == -1 || close(pipefd[1]) == -1) {
            perror("close");
            return 1;
        }

        // Execute the second command
        execvp(li->cmds[1].args[0], li->cmds[1].args);
        perror("execvp");
        return 1;
    }

    // Parent process

    // Close unused pipe ends
    if (close(pipefd[0]) == -1 || close(pipefd[1]) == -1) {
        perror("close");
        return 1;
    }

    // Wait for both child processes to finish and print their status
    for (int i = 0; i < 2; i++) {
        if (li->background) {
            // Add background process to the list
            bg_processes[bg_index++] = pids[i];
        } else {
            // Add foreground process to the list
            fg_processes[fg_index++] = pids[i];
            // Wait for the foreground process to complete
            while (fg_index > 0) {
                int status;
                pid_t res = wait(&status);
                if (res == -1) {
                    perror("wait");
                    return 1;
                } else {
                    print_process_status(res, status, li->background);
                    remove_fg_process(res);
                }
            }
        }
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
            if (li->background) {
                // Redirect standard input to /dev/null for background processes
                if (!is_input_redirected()) {
                    if (redirect_input_to_dev_null() != 0) {
                        return 1;
                    }
                }
            } else {
                // Reset SIGINT handler to default for foreground commands
                struct sigaction default_sigint;
                sigemptyset(&default_sigint.sa_mask);
                default_sigint.sa_flags = SA_RESTART;
                default_sigint.sa_handler = SIG_DFL;
                if (sigaction(SIGINT, &default_sigint, NULL) == -1) {
                    perror("sigaction");
                    return 1;
                }
            }

            // Redirect input
            if (i > 0) {
                if (dup2(pipefd[(i - 1) * 2], STDIN_FILENO) == -1) {
                    perror("dup2");
                    return 1;
                }
            }

            // Redirect output
            if (i < li->n_cmds - 1) {
                if (dup2(pipefd[i * 2 + 1], STDOUT_FILENO) == -1) {
                    perror("dup2");
                    return 1;
                }
            }

            // Close all pipe descriptors in child
            for (size_t j = 0; j < 2 * (li->n_cmds - 1); j++) {
                if (close(pipefd[j]) == -1) {
                    perror("close");
                    return 1;
                }
            }

            // Execute the command
            execvp(li->cmds[i].args[0], li->cmds[i].args);
            perror("execvp");
            return 1;
        }
    }

    // Close all pipe descriptors in parent
    for (size_t i = 0; i < 2 * (li->n_cmds - 1); i++) {
        if (close(pipefd[i]) == -1) {
            perror("close");
            return 1;
        }
    }

    // Wait for all child processes
    for (size_t i = 0; i < li->n_cmds; i++) {
        if (li->background) {
            // Add background process to the list
            bg_processes[bg_index++] = pids[i];
        } else {
            // Add foreground process to the list
            fg_processes[fg_index++] = pids[i];
            // Wait for the foreground process to complete
            while (fg_index > 0) {
                int status;
                pid_t res = wait(&status);
                if (res == -1) {
                    perror("wait");
                    return 1;
                } else {
                    print_process_status(res, status, li->background);
                    remove_fg_process(res);
                }
            }
        }
    }

    return 0;
}
