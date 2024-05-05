#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

int execute_command_extern(char *cmd, char **args) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }
    if (pid == 0) {
        execvp(cmd, args);
        char error_message[256];
        sprintf(error_message, "Recouvrement: %s", cmd);
        perror(error_message);
        return 1;
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return 1;
        }
        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            fprintf(stderr, "        FG : %d exited, status=%d\n", pid, exit_status);
        } else if (WIFSIGNALED(status)) {
            int signal_number = WTERMSIG(status);
            fprintf(stderr, "        FG : %d terminated by signal %d\n", pid, signal_number);
        }
    }
    return 0;
}
