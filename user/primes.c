#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void 
handle(int lfd[]);

int
main(int argc, char *argv[]) {
    int fd[2];
    pipe(fd);
    for(int i = 2; i <= 35; i++) {
        write(fd[1], &i, 4);
    }
    close(fd[1]);
    handle(fd);
    exit(0);
}

void 
handle(int lfd[]) {
    int rfd[2];
    int pid;
    int num, next;

    if (read(lfd[0], &num, 4) == 0) {
        exit(0);
    }
    
    if(pipe(rfd) == -1) goto err;

    printf("prime %d\n", num);

    while(read(lfd[0], &next, 4) != 0) {
        if (next % num != 0) {
            write(rfd[1], &next, 4);
        }
    }
    close(lfd[0]); // close left
    close(rfd[1]); // close right
    
    if((pid = fork()) == -1) goto err;

    if(pid == 0) { // child
        handle(rfd);
    } else { // parent
        // printf("rfd = [%d, %d]\n", rfd[0], rfd[1]);
        while(wait(0) != -1);
    }

    return;

    err:
    printf("pipe or fork error\n");
    close(lfd[0]); // close left
    close(rfd[1]);
    
}