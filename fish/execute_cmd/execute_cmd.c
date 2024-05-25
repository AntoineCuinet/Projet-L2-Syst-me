#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "cmdline.h"
#include "util.h"
#include "intern_cmd/intern_cmd.h"
#include "pipe_cmd/pipe_cmd.h"

volatile pid_t bg_processes[MAX_CMDS];
volatile size_t bg_index = 0;
volatile pid_t fg_processes[MAX_CMDS];
volatile size_t fg_index = 0;


/**
 * @brief Remove terminated background processes from the array.
 *
 * This function checks the background processes array and removes any processes
 * that have terminated.
 */
void remove_terminated_bg_process() {
    size_t i = 0;
    while (i < MAX_CMDS) {
        if (bg_processes[i] == -1) {
            remove_element(bg_processes, MAX_CMDS, i);
            bg_index--;
        } else {
            i++;
        }
    }
}

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
void remove_fg_process(pid_t pid_to_remove) {
    for (size_t i = 0; i < MAX_CMDS; ++i) {
        if (fg_processes[i] == pid_to_remove) {
            remove_element(fg_processes, MAX_CMDS, i);
            fg_index--;
        }
    }
}


/**
 * @brief Signal handler for SIGCHLD.
 *
 * This function handles the SIGCHLD signal, which indicates that a child process
 * has terminated. It cleans up the terminated background processes.
 *
 * @param signal The signal number.
 */
void signal_handler(int signal) {
    if (signal == SIGCHLD) {
        int status;
        int pid_wait;

        // Clean up zombie processes
        for (size_t i = 0; i < bg_index; ++i){
            if (bg_processes[i] >= 1 && (pid_wait = waitpid(bg_processes[i], &status, WNOHANG)) > 0 ) {
                print_process_status(pid_wait, status, 1);
                bg_processes[i] = -1;
            }
        }
        remove_terminated_bg_process();
    }
}

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
 * @param li The line struct containing the parsed command line.
 *
 * @return 0 on success, 1 on error with error messages printed to stderr.
 *
 * Example usage:
 * @code
 * char *args[] = {"ls", "-l", NULL};
 * execute_command("ls", args, 0); // Run 'ls -l' in the foreground.
 * @endcode
 */
int execute_command(char *cmd, char **args, int bg, struct line *li) {
    // Check if the command is a cd or exit command
    if (strcmp(cmd, "cd") == 0) {
        int ret = execute_command_intern_cd(li->cmds[0].args);
        if (ret != 0) {
            return 1;
        }
    } else if (strcmp(cmd, "exit") == 0) {
        int ret = execute_command_intern_exit(li, &li->cmds[0]);
        if (ret != 0) {
            return 1;
        }
    }


    // Handle pipes
    if (li->n_cmds > 1) {
        return execute_line_with_one_pipes(li);

    // Execute external command
    } else {

        pid_t pid = fork();
        
        if (pid == -1) {
            perror("fork");
            return 1;
        }

        if (pid == 0) { // Child processes
            if (bg) {
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
        
            // Execute the command
            execvp(cmd, args);
            char error_message[256];
            snprintf(error_message, 256, "Exec error: %s", cmd);
            perror(error_message);
            return 1;
        } else {
            if (bg) {
                // Add background process to the list
                bg_processes[bg_index++] = pid;
            } else {
                // Add foreground process to the list
                fg_processes[fg_index++] = pid;

                // Wait for the foreground process to complete
                while (fg_index > 0) {
                    int status;
                    pid_t res = wait(&status);
                    if (res == -1) {
                        perror("wait");
                        return 1;
                    } else {
                        print_process_status(res, status, bg);
                        remove_fg_process(res);
                    }
                }
            }
        }
    }
    return 0;
}
