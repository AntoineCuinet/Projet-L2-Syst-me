#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "util.h"

volatile pid_t fg_processes[MAX_CMDS];
volatile size_t fg_index = 0;


void remove_fg_process(pid_t pid_to_remove) {
  for (size_t i = 0; i < MAX_CMDS; ++i) {
    if (fg_processes[i] == pid_to_remove) {
      remove_element(fg_processes, MAX_CMDS, i);
      fg_index--;
    }
  }
}

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

        while (fg_index > 0) {
            int status;
            pid_t res = wait(&status);
            if (res == -1) {
                perror("wait");
                return 1;
            } else {
                print_process_status(pid, status, bg);
                remove_fg_process(res);
            }
        }
    }
    return 0;
}
