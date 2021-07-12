#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

int main () {
    int pid;
    int fd = open("flag", O_RDWR);
    if (fd < 0) {
        perror("parent: open error");
        exit(errno);
    }
    struct flock flk;
    flk.l_type = F_WRLCK;
    flk.l_whence = SEEK_SET;
    flk.l_start = 0; // 锁整个文件
    flk.l_len = 0;
    
    if (fcntl(fd, F_SETLKW, &flk) <0) {
        perror("parent: fcntl error");
        exit(errno);
    }

    if ((pid = fork()) < 0) {
        perror("parent: fork error");
        exit(errno);
    } else if (pid > 0) {
        printf("parent: get write lock, sleep 2...\n");
        sleep(10);
        printf("parent: release write lock.\n");
        flk.l_type = F_UNLCK;
        flk.l_whence = SEEK_SET;
        flk.l_start = 0;
        flk.l_len = 0;
        if (fcntl(fd, F_SETLK, &flk) < 0) {
            perror("parent: fcntl error");
            exit(errno);
        }
        waitpid(pid, NULL, 0); // TODO: 记录一下waitpid的用法
        exit(0);
    } else {
        printf("child:  wait to getting write lock...\n");
        while (1){
            flk.l_type = F_WRLCK;
            flk.l_whence = SEEK_SET;
            flk.l_start = 0;
            flk.l_len = 0;
            if (fcntl(fd, F_SETLK, &flk) <0) {
                perror("child:  fcntl error");
                sleep(1);
            } else {
                break;
            }
        }
        printf("child:  get write lock!\n");
        exit(0);
    }
    return 0;
}
