#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

void find(char* path, char* name);

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("invalid arguments\n");
        exit(0);
    }
    find(argv[1], argv[2]);

    exit(0);
}

void find(char* path, char* name) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if (st.type == T_DIR) {
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
            printf("find: path too long\n");
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0) {
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            switch(st.type) {
                case T_DIR:
                    find(buf, name);
                    break;
                case T_FILE:
                    if(strcmp(de.name, name) == 0) printf("%s\n", buf);
                    break;
            }
        }
    } else {
        fprintf(2, "find: path should be a dir\n");
    }

    close(fd);
}