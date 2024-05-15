#ifndef REDIRECT_COMMAND_H
#define REDIRECT_COMMAND_H

int redirect_input(char *filename);
int redirect_output_trunc(char *filename);
int redirect_output_append(char *filename);


#endif /* REDIRECT_COMMAND_H */