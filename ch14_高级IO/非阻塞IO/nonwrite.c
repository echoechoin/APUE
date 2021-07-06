#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

char     buf[50000000];

int set_block(int fd) {
    int flags;
    if(flags = fcntl(fd, F_GETFL, 0) < 0)
    {
        perror("fcntl");
        return -1;
    }
    flags |= O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flags) < 0)
    {
        perror("fcntl");
        return -1;
    }
    return 0;
}
int clr_block(int fd) {
    int flags;
    if(flags = fcntl(fd, F_GETFL, 0) < 0)
    {
        perror("fcntl");
        return -1;
    }
    flags &= ~O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flags) < 0)
    {
        perror("fcntl");
        return -1;
    }
    return 0;
}

int
main(void)
{
    int    ntowrite, nwrite;
    char    *ptr;

    ntowrite = read(STDIN_FILENO, buf, sizeof(buf)); // 读取数据
    fprintf(stderr, "read %d bytes\n", ntowrite);

    set_block(STDOUT_FILENO);    /* set nonblocking */

    ptr = buf;
    while(ntowrite > 0)                   /* 这种形式的循环称为轮循，在多用户系统上它浪费了CPU时间。*/
    {
        errno = 0;
        nwrite = write(STDOUT_FILENO, ptr, ntowrite);
        fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno); // 写到标准输出的话，就可能会报错：errno = 11 表示EAGAIN

        if(nwrite > 0)
        {
            ptr += nwrite;
            ntowrite -= nwrite;
        }
    }
    clr_block(STDOUT_FILENO);    /* clear nonblocking */

    exit(0);
}