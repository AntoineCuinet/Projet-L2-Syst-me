#ifndef PROCESS_MANAGEMENT_H
#define PROCESS_MANAGEMENT_H

#include <sys/types.h>

#define MAX_CMDS 256

extern volatile pid_t bg_processes[MAX_CMDS];
extern volatile int bg_index;
extern volatile pid_t fg_processes[MAX_CMDS];
extern volatile int fg_index;

#endif // PROCESS_MANAGEMENT_H
