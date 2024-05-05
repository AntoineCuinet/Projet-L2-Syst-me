/*********************************************************************/
/***                             FISH                              ***/
/***                    CUINET ANTOINE TP2A-CMI                    ***/
/***               Système et programmation système                ***/
/***                        L2 Informatique                        ***/
/***                         UFC - UFR ST                          ***/
/*********************************************************************/



/*********************************************************************/
/***                          CONCLUSION                           ***/
/***                                                               ***/
/***                                                               ***/
/***                                                               ***/
/***                                                               ***/
/***                                                               ***/
/*********************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "cmdline.h"
#include "extern_cmd/extern_cmd.h"
#include "intern_cmd/intern_cmd.h"

#define BUFLEN 512

#define YES_NO(i) ((i) ? "Y" : "N")

int main() {
  struct line li;
  char buf[BUFLEN];

  line_init(&li);

  for (;;) {
    printf("fish> ");
    fgets(buf, BUFLEN, stdin);

    int err = line_parse(&li, buf);
    if (err) { 
      //the command line entered by the user isn't valid
      line_reset(&li);
      continue;
    }

    // fprintf(stderr, "Command line:\n");
    // fprintf(stderr, "\tNumber of commands: %zu\n", li.n_cmds);

    // for (size_t i = 0; i < li.n_cmds; ++i) {
    //   fprintf(stderr, "\t\tCommand #%zu:\n", i);
    //   fprintf(stderr, "\t\t\tNumber of args: %zu\n", li.cmds[i].n_args);
    //   fprintf(stderr, "\t\t\tArgs:");
    //   for (size_t j = 0; j < li.cmds[i].n_args; ++j) {
    //     fprintf(stderr, " \"%s\"", li.cmds[i].args[j]);
    //   }
    //   fprintf(stderr, "\n");
    // }

    // fprintf(stderr, "\tRedirection of input: %s\n", YES_NO(li.file_input));
    // if (li.file_input) {
    //   fprintf(stderr, "\t\tFilename: '%s'\n", li.file_input);
    // }

    // fprintf(stderr, "\tRedirection of output: %s\n", YES_NO(li.file_output));
    // if (li.file_output) {
    //   fprintf(stderr, "\t\tFilename: '%s'\n", li.file_output);
    //   fprintf(stderr, "\t\tMode: %s\n", li.file_output_append ? "APPEND" : "TRUNC");
    // }

    // fprintf(stderr, "\tBackground: %s\n", YES_NO(li.background));

    /* do something with li */


    if (strcmp(li.cmds[0].args[0], "cd") == 0) {
      execute_command_intern_cd(li.cmds[0].args);
    } else if (strcmp(li.cmds[0].args[0], "exit") == 0) {
      execute_command_intern_exit();
    } 
    // else if (strcmp(li.cmds[0].args[0], "help") == 0) {
    //   help_command();
    // }
    else {
      int result = execute_command_extern(li.cmds[0].args[0], li.cmds[0].args);
      if (result != 0) {
        return 1;
      }
    }

    




    line_reset(&li);
  }

  return 0;
}

