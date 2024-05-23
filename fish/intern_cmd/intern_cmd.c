#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <errno.h>
#include <pwd.h>
#include "cmdline.h"


/**
 * @brief Change the current working directory.
 *
 * This function implements the 'cd' command for the shell. It handles various forms
 * of the 'cd' command, including 'cd', 'cd ~', 'cd ~user', 'cd ~user/path', and 'cd path'.
 *
 * @param args Array of arguments where args[0] is "cd" and args[1] is the target directory.
 * @return int Returns 0 on success, or 1 on failure.
 */
int execute_command_intern_cd(char **args) {
    char target_dir[1024];  // Buffer to store the target directory path
    struct passwd *pw;

    // Check if the cd command has too many arguments
    if (args[2] != NULL) {
        fprintf(stderr, "cd: too many arguments\n");
        return 1;
    }
    
    // Check if the cd command has an argument
    if (args[1] == NULL || strcmp(args[1], "~") == 0 || (args[1][0] == '~' && strlen(args[1]) == 1)) {
        // If no argument, ~, or ~ with no username is provided, change to the home directory
        snprintf(target_dir, sizeof(target_dir), "%s", getenv("HOME"));
    } else if (args[1][0] == '~') {
        // Handle ~user and ~user/path cases
        char *username_end = strchr(args[1], '/');
        if (username_end == NULL) {
            username_end = args[1] + strlen(args[1]);
        }

        char username[1024];
        strncpy(username, args[1] + 1, username_end - args[1] - 1);
        username[username_end - args[1] - 1] = '\0';

        pw = getpwnam(username);
        if (pw == NULL) {
            fprintf(stderr, "The user \"%s\" doesn't exist\n", username);
            return 1;
        }

        if (*username_end == '\0') {
            snprintf(target_dir, sizeof(target_dir), "%s", pw->pw_dir);
        } else {
            snprintf(target_dir, sizeof(target_dir), "%s%s", pw->pw_dir, username_end);
        }

        printf("Debug: dir = %s\n", target_dir);
    } else {
        // Otherwise, change to the specified directory
        snprintf(target_dir, sizeof(target_dir), "%s", args[1]);
    }

    // Attempt to change the directory
    if (chdir(target_dir) != 0) {
        if (errno == EACCES) {
            fprintf(stderr, "chdir : permission denied\n");
        } else {
            perror("fish");
        }
        return 1;
    }

    return 0;
}


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
int execute_command_intern_exit(struct line *li, struct cmd *cmd) {
    // Check if the exit command has too many arguments
    if (cmd->n_args > 2) {
        fprintf(stderr, "exit: too many arguments\n");
        return 1;
    }
    printf("Exiting fish shell...\n");
    int exit_status = EXIT_SUCCESS;
    if (cmd->n_args == 2) {
        exit_status = atoi(cmd->args[1]);
    }
    line_reset(li);
    exit(exit_status);
}