#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main () {
    int file1_fd = open("./file1", O_RDWR);
    if (file1_fd < 0) {
        perror("open file1 error");
        exit(errno);
    }
    fd_set rd_set, wt_set, err_set;
    struct timeval tv;
    while (1) {
        // 每次需要重新设置关心的行为！
        FD_ZERO(&rd_set);
        FD_ZERO(&wt_set);
        FD_ZERO(&err_set);
        // FD_SET(file1_fd, &rd_set);
        // FD_SET(file1_fd, &wt_set);
        // FD_SET(file1_fd, &err_set);

        // 每次需要重新设置超时时间
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        int nums = select(file1_fd+1, &rd_set, &wt_set, &err_set, &tv);
        if (nums < 0) {
            perror("select error");
            exit(errno);
        }
        if (nums == 0) {
            printf("no fd is prepared.\n");
        }
        if (FD_ISSET(file1_fd, &rd_set)) {
            printf("file1 is readable.\n");
        } if (FD_ISSET(file1_fd, &wt_set)) {
            printf("file1 is writeable.\n");
        } if (FD_ISSET(file1_fd, &err_set)) {
            printf("file1 fd has error.\n");
        }

        // timeout被修改了！
        printf("sec: %d\n", tv.tv_sec);
        printf("usec: %d\n", tv.tv_usec);
    }

    return 0;
}