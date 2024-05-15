#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include "cmdline.h"


int execute_command_intern_cd(char **args) {
    char* target_dir;

    // Check if the cd command has an argument
    if (args[1] == NULL || strcmp(args[1], "~") == 0) {
        // If no argument or ~ is provided, change to the home directory
        target_dir = getenv("HOME");
    } else {
        // Otherwise, change to the specified directory
        target_dir = args[1];
    }

    // Attempt to change the directory
    if (chdir(target_dir) != 0) {
        perror("fish");
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