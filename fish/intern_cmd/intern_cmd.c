#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <errno.h>
#include <pwd.h>
#include "cmdline.h"


int execute_command_intern_cd(char **args) {
    char target_dir[1024];  // Buffer to store the target directory path
    struct passwd *pw;
    
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


int execute_command_intern_exit(struct line *li, struct cmd *cmd) {
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