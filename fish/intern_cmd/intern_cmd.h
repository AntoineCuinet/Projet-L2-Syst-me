#ifndef EXECUTE_COMMAND_INTERN_H
#define EXECUTE_COMMAND_INTERN_H


int execute_command_intern_cd(char **args);
int execute_command_intern_exit(struct line *li, struct cmd *cmd);

#endif /* EXECUTE_COMMAND_INTERN_H */