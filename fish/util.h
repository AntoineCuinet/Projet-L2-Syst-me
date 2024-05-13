#ifndef UTIL_H
#define UTIL_H

char* get_current_dir_name(int BUFLEN);
void update_prompt(int BUFLEN);
void print_process_status(pid_t pid, int status, int is_background);


#endif /* UTIL_H */