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
#include <libgen.h>
#include <signal.h>

#include "cmdline.h"
#include "util.h"
#include "extern_cmd/extern_cmd.h"
#include "intern_cmd/intern_cmd.h"
#include "redirect_cmd/redirect_cmd.h"
#include "background_cmd/background_cmd.h"

#define BUFLEN 512

#define YES_NO(i) ((i) ? "Y" : "N")

volatile pid_t pid = -1;


void signal_handler(int signal) {
  if (signal == SIGINT && pid != -1) {
    // pid = 0 => ne rien faire / autres => tuer le processus dans un for
    // TODO: Mettre ici le code pour terminer les commandes lancées en avant-plan
    kill(pid, SIGTERM);
    // buf[0] = '\n';
    // buf[1] = '0';
  } 
  //else {
    // ignorer le signal
    //signal(signal, SIG_IGN);
  //}

  if (signal == SIGCHLD) {
        // Nettoyer les processus zombies
        int status;
        while (waitpid(-1, &status, WNOHANG) > 0) {
            if (WIFEXITED(status)) {
                printf("Child process exited with status: %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("Child process terminated by signal: %d\n", WTERMSIG(status));
            }
        }
    }
}





int main() {
  struct line li;
  char buf[BUFLEN];

  // Install signal handler
  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = signal_handler;
  sa.sa_flags = 0;
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("sigaction");
    return 1;
  } 
  // else {
  //   buf[0] = '\n';
  //   buf[1] = 0;
  // }


  line_init(&li);

  for (;;) {
    update_prompt(BUFLEN);
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


    int saved_stdin;
    int saved_stdout;
    int saved_stderr;

    saved_stdin = dup(STDIN_FILENO);
    saved_stdout = dup(STDOUT_FILENO);
    saved_stderr = dup(STDERR_FILENO);


    if (li.n_cmds > 0) {
      // Check if there is an input redirection
      if (li.file_input) {
        if (redirect_input(li.file_input) != 0) {
          return 1;
        }
      }

      // Checks if there is output redirection in TRUNC mode
      if (li.file_output && !li.file_output_append) {
        if (redirect_output_trunc(li.file_output) != 0) {
          return 1;
        }
      }

      // Checks if there is output redirection in APPEND mode 
      if (li.file_output && li.file_output_append) {
        if (redirect_output_append(li.file_output) != 0) {
          return 1;
        }
      }



      if (strcmp(li.cmds[0].args[0], "cd") == 0) {
        execute_command_intern_cd(li.cmds[0].args);
      } else if (strcmp(li.cmds[0].args[0], "exit") == 0) {
        execute_command_intern_exit(&li, &li.cmds[0]);
      } else if (li.background) {
        int result = background_command(li.cmds[0].args[0], li.cmds[0].args, pid, li.background);
        if (result != 0) {
          return 1;
        }
      } else {
        int result = execute_command_extern(li.cmds[0].args[0], li.cmds[0].args, pid, li.background);
        if (result != 0) {
          return 1;
        }
      }
    }
    

    dup2(saved_stdin, STDIN_FILENO);
    dup2(saved_stdout, STDOUT_FILENO);
    dup2(saved_stderr, STDERR_FILENO);

    // Fermeture des descripteurs dupliqués
    close(saved_stdin);
    close(saved_stdout);
    close(saved_stderr);


    line_reset(&li);
  }

  return 0;
}
