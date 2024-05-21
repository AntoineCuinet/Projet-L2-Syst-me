#ifndef UTIL_H
#define UTIL_H

#include <sys/types.h>
#include "cmdline.h"

extern volatile pid_t bg_processes[MAX_CMDS];
extern volatile int bg_index;
extern volatile pid_t fg_processes[MAX_CMDS];
extern volatile int fg_index;

void signal_handler(int signal);
char* get_current_dir_name();
void update_prompt();
void print_process_status(pid_t pid, int status, int is_background);


#endif /* UTIL_H */