#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "util.h"


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
        return 1;
    }

    close(dev_null_fd);
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
        sprintf(error_message, "Recouvrement: %s", cmd);
        perror(error_message);
        return 1;
    } else { // Processus parent
        int status;
        if (! bg) {
            if (waitpid(pid, &status, 0) == -1) {
                perror("waitpid");
                return 1;
            }
            print_process_status(pid, status, bg);
        }
    }
    return 0;
}
