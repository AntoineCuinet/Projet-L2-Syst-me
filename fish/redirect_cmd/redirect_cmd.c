#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

void redirect_input(char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (dup2(fd, STDIN_FILENO) == -1) {
        perror("dup2");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
}

void redirect_output_trunc(char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (dup2(fd, STDOUT_FILENO) == -1) {
        perror("dup2");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
}

void redirect_output_append(char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (dup2(fd, STDOUT_FILENO) == -1) {
        perror("dup2");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
}