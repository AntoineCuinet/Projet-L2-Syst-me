#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include "util.h"

int execute_command_extern(char *cmd, char **args, pid_t pid, int bg) {
    pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }
    if (pid == 0) {
        execvp(cmd, args);
        char error_message[256];
        snprintf(error_message, 256, "Recouvrement: %s", cmd);
        perror(error_message);
        return 1;
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return 1;
        }
        print_process_status(pid, status, bg);
    }
    return 0;
}
