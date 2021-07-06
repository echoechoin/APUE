#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

int main () {
    int fd = open("./file", O_RDWR);
    if (fd < 0) {
        perror("open() error");
        exit(errno);
    }
    struct flock flk;
    flk.l_type = F_WRLCK;
    flk.l_whence = SEEK_SET;
    flk.l_start = 0; // 锁整个文件
    flk.l_len = 0;
    while (fcntl(fd, F_SETLK, &flk) < 0) {
        perror("fcntl()error");
        sleep(1);
    }
    printf("get file lock, sleep 10 sec.\n");
    sleep(10);
    exit(0); // 释放锁并退出程序
}