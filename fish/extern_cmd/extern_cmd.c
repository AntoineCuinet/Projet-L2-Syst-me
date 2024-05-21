#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "util.h"
// #include "process_management.h"

volatile pid_t fg_processes[MAX_CMDS];
volatile int fg_index = 0;

int execute_command_extern(char *cmd, char **args, pid_t pid, int bg) {
    pid = fork();
    
    if (pid == -1) {
        perror("fork");
        return 1;
    }
    if (pid == 0) {
        struct sigaction default_sigint;
        sigemptyset(&default_sigint.sa_mask);
        default_sigint.sa_flags = SA_RESTART;
        default_sigint.sa_handler = SIG_DFL;
        if (sigaction(SIGINT, &default_sigint, NULL) == -1) {
            perror("sigaction");
            return 1;
        }
        
        execvp(cmd, args);
        char error_message[256];
        snprintf(error_message, 256, "Recouvrement: %s", cmd);
        perror(error_message);
        return 1;
    } else {
        fg_processes[fg_index++] = pid;
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return 1;
        }
        print_process_status(pid, status, bg);
    }
    return 0;
}
