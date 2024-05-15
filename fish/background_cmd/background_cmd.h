#ifndef BACKGROUND_COMMAND_H
#define BACKGROUND_COMMAND_H

int is_input_redirected();

int redirect_input_to_dev_null();

int background_command(char *cmd, char **args, pid_t pid, int bg);

#endif /* BACKGROUND_COMMAND_H */