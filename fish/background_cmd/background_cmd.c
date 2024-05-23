#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "util.h"

volatile pid_t bg_processes[MAX_CMDS];
volatile size_t bg_index = 0;


int is_input_redirected() {
    // Vérifier si l'entrée standard est redirigée
    return !isatty(STDIN_FILENO);
}

int redirect_input_to_dev_null() {
    int dev_null_fd = open("/dev/null", O_RDONLY);
    if (dev_null_fd == -1) {
        perror("Error opening /dev/null");
        return 1;
    }

    if (dup2(dev_null_fd, STDIN_FILENO) == -1) {
        perror("Error redirecting input to /dev/null");
        close(dev_null_fd);
        return 1;
    }

    if (close(dev_null_fd) == -1) {
        perror("close");
        return 1;
    }
    return 0;
}


int background_command(char *cmd, char **args, pid_t pid, int bg) {
    pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) { // Processus enfant

        // Rediriger l'entrée standard vers /dev/null si nécessaire pour les processus en arrière-plan afin qu'ils ne bloquent pas le shell
        if (!is_input_redirected() && bg) {
            if (redirect_input_to_dev_null() != 0) {
                return 1;
            }
        }

        execvp(cmd, args);
        char error_message[256];
        snprintf(error_message, 256, "Recouvrement: %s", cmd);
        perror(error_message);
        return 1;
    }
    bg_processes[bg_index++] = pid;

    return 0;
}
