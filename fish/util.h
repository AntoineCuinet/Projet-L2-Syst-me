#ifndef UTIL_H
#define UTIL_H

#include <sys/types.h>
#include "cmdline.h"

extern volatile pid_t bg_processes[MAX_CMDS];
extern volatile size_t bg_index;
extern volatile pid_t fg_processes[MAX_CMDS];
extern volatile size_t fg_index;

char* get_current_dir_name();
void update_prompt();
void remove_element(volatile pid_t *array, int size, size_t index);
void remove_terminated_bg_process();
void print_process_status(pid_t pid, int status, int is_background);
void signal_handler(int signal);


#endif /* UTIL_H */