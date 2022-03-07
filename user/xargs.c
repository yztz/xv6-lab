#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
    int pid;
    int i;
    char buf[512], *p;
    char* args[MAXARG];
    char c;
    p = buf;

    for(i = 0; i < argc - 1; i++) {
        args[i] = (char *)malloc(strlen(argv[i + 1]) + 1);
        strcpy(args[i], argv[i + 1]);
    }
    // printf("%d\n", i);

    while(read(0, &c, 1) != 0) {
        // printf("read c \n");
        if (c == '\n') {
            *p = '\0';
            p = buf;
            // printf("buf = %s\n", buf);
            args[i] = (char *)malloc(strlen(buf) + 1);
            strcpy(args[i], buf);
            args[i + 1] = 0;
            if((pid = fork()) == -1) {
                fprintf(2, "xagrs: fork error\n");
                exit(0);
            }
            if(pid == 0) { // child
                // printf("execing...\n");
                exec(argv[1], args);
            } else { // parent
                wait(0);
                free(args[i]);
            }
        } else {
            *p++ = c;
        }
    }
    exit(0);
}