#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>



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


int execute_command_intern_exit() {
    printf("Exiting fish shell...\n");
    exit(EXIT_SUCCESS);
}