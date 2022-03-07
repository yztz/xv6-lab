#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
    if(argc == 1) {
        write(1, "invaild argument\n", strlen("invaild argument\n"));
        exit(0);
    }
    sleep(atoi(argv[1]));
    exit(0);
}