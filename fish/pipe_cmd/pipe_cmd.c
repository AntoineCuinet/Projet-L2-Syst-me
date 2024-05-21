#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "cmdline.h"

int execute_line_with_pipes(struct line *li) {
    int pipefd[2 * (li->n_cmds - 1)];
    pid_t pids[li->n_cmds];

    // Créer les tubes nécessaires
    for (size_t i = 0; i < li->n_cmds - 1; i++) {
        if (pipe(pipefd + i * 2) == -1) {
            perror("pipe");
            return 1;
        }
    }

    for (size_t i = 0; i < li->n_cmds; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork");
            return 1;
        }

        if (pids[i] == 0) {
            // Processus enfant

            // Rediriger l'entrée
            if (i > 0) {
                if (dup2(pipefd[(i - 1) * 2], STDIN_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            // Rediriger la sortie
            if (i < li->n_cmds - 1) {
                if (dup2(pipefd[i * 2 + 1], STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            // Fermer tous les descripteurs de tubes dans l'enfant
            for (size_t j = 0; j < 2 * (li->n_cmds - 1); j++) {
                close(pipefd[j]);
            }

            // Exécuter la commande
            execvp(li->cmds[i].args[0], li->cmds[i].args);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }

    // Fermer tous les descripteurs de tubes dans le parent
    for (size_t i = 0; i < 2 * (li->n_cmds - 1); i++) {
        close(pipefd[i]);
    }

    // Attendre tous les processus enfants
    for (size_t i = 0; i < li->n_cmds; i++) {
        int status;
        waitpid(pids[i], &status, 0);
    }

    return 0;
}
