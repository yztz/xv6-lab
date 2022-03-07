#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
    int pipe1[2];
    int pipe2[2];
    int pid;
    if(pipe(pipe1) == -1 || pipe(pipe2) == -1 || (pid = fork()) == -1) {
        write(1, "unexpected error", strlen("unexpected error"));
        exit(0);
    }
    char byte;
    if(pid == 0) { // child
        read(pipe1[0], &byte, 1);
        printf("%d: received ping\n", getpid());
        write(pipe2[1], &byte, 1);
    } else { // parent
        byte = 'a';
        write(pipe1[1], &byte, 1);
        read(pipe2[0], &byte, 1);
        printf("%d: received pong\n", getpid());
    }

    exit(0);
}