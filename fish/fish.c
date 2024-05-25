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
// #include "intern_cmd/intern_cmd.h"
#include "redirect_cmd/redirect_cmd.h"
#include "pipe_cmd/pipe_cmd.h"
#include "execute_cmd/execute_cmd.h"

#define BUFLEN 512

#define YES_NO(i) ((i) ? "Y" : "N")


int main() {
  struct line li;
  char buf[BUFLEN];

  // Install signal handler for SIGINT
  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = SIG_IGN;
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("sigaction");
    return 1;
  }

  // Install signal handler for SIGCHLD
  struct sigaction sigchld_action;
  sigemptyset(&sigchld_action.sa_mask);
  sigchld_action.sa_handler = signal_handler;
  sigchld_action.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sigchld_action, NULL) == -1) {
    perror("sigaction");
    return 1;
  }

  line_init(&li);

  for (;;) {
    update_prompt();
    fgets(buf, BUFLEN, stdin);

    int err = line_parse(&li, buf);
    if (err) { 
      // The command line entered by the user isn't valid
      line_reset(&li);
      continue;
    }


    // Creating a copy of the file descriptor (useful for redirects from question 5)
    int saved_stdin = dup(STDIN_FILENO);
    int saved_stdout = dup(STDOUT_FILENO);
    int saved_stderr = dup(STDERR_FILENO);

    if (saved_stdin == -1 || saved_stdout == -1 || saved_stderr == -1) {
        perror("dup");
        exit(EXIT_FAILURE);
    }


    // Check if there are commands to execute
    if (li.n_cmds > 0) {

      // Check if there is an input redirection
      if (li.file_input && redirect_input(li.file_input) != 0) {
        return 1;
      }
      // Checks if there is output redirection in TRUNC mode
      if (li.file_output && !li.file_output_append && redirect_output_trunc(li.file_output) != 0) {
        return 1;
      }
      // Checks if there is output redirection in APPEND mode
      if (li.file_output && li.file_output_append && redirect_output_append(li.file_output) != 0) {
        return 1;
      }

      // Execute the command
      int result = execute_command(li.cmds[0].args[0], li.cmds[0].args, li.background, &li);
      if (result != 0) {
        return 1;
      }
    }
    

    // Restore and close standard file descriptors
    if (dup2(saved_stdin, STDIN_FILENO) == -1 ||
      dup2(saved_stdout, STDOUT_FILENO) == -1 ||
      dup2(saved_stderr, STDERR_FILENO) == -1) {
      perror("dup2");
      exit(EXIT_FAILURE);
    }
    close(saved_stdin);
    close(saved_stdout);
    close(saved_stderr);

    line_reset(&li);
  }
  return 0;
}
