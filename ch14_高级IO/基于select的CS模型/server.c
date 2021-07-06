// TODO 我放弃了 23333
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define IP "0.0.0.0"
#define PORT 8888

int main () {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if ( fd < 0 ) {
        perror("socket error");
        exit(errno);
    }
    struct sockaddr_in sa, ca;
    sa.sin_family = AF_INET;
    sa.sin_port   = htons(PORT);
    inet_aton(IP, &(sa.sin_addr));

    if (bind(fd, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
        perror("bind error");
        exit(errno);
    }

    if (listen(fd, 10) < 0) {
        perror("listen error");
        exit(errno);
    }
    printf("listen..\n");

    while(1) {
        fd_set rfd;
        fd_set wfd;
        fd_set efd;

        FD_ZERO(&rfd);
        FD_ZERO(&wfd);
        FD_ZERO(&efd);
        for (int i = 3; i < 1024; i++){
            FD_SET(i,&rfd);
            FD_SET(i,&wfd);
            FD_SET(i,&efd);
        }
        int wait_for_process = 0;
        if ((wait_for_process = select(FD_SETSIZE, &rfd, &wfd, &efd, NULL)) < 0) {
            perror("select error");
            exit(errno);
        } else if (wait_for_process == 0) {
            continue;
        } else {
            for(int i = 3; i < 1024; i++){
                if (FD_ISSET(i, &rfd)) {
                    printf("readable\n");
                    if (i == fd) {
                        int cfd = accept(fd, (struct sockaddr*)&ca, sizeof(ca));
                        if (cfd >= 1024) {
                            printf("connect number exceed 1024\n");
                            close(cfd);
                        }
                    }
                    else {
                        // read
                        
                    }
                }
                if (FD_ISSET(i, &wfd)) {
                    printf("writeable\n");
                    // write
                    write(i,"hello world!",sizeof("hello world!"));
                }
                if (FD_ISSET(i, &efd)) {
                    printf("error fd\n");
                }
            }
        }
    }
    

    return 0;
}