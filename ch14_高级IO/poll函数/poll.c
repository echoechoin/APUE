#include <poll.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <stdlib.h>

#define MAX_POLL_FD 2

int main () {
    int fd = open("/dev/null", O_RDWR);
    if (fd < 0) {
        perror("open error");
    }
    struct pollfd pfds[MAX_POLL_FD];
    while(1) {
        // 是否可读
        pfds[0].events = POLLIN;
        pfds[0].fd = fd;
        pfds[0].revents = 0;

        // 是否可写
        pfds[1].events = POLLOUT;
        pfds[1].fd = fd;
        pfds[1].revents = 0;

        poll(pfds, MAX_POLL_FD, 0);
        if (pfds[0].revents == POLLIN) {
            printf("fd is readable\n");
        } 
        if (pfds[1].revents == POLLOUT) {
            printf("fd is writeable\n");
        }
        sleep(2);
    }

    return 0;
}