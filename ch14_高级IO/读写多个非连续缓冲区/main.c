#include "sys/uio.h"
#include "stdio.h"

int main() {
    char buff1[1024] = "hello ";
    char buff2[1024] = "world!\n";
    struct iovec iovector[2];
    iovector[0].iov_base = buff1;
    iovector[0].iov_len = 1024;

    iovector[1].iov_base = buff2;
    iovector[1].iov_len = 1024;
    // 直接将两个缓冲区写到标准输出，比两次write快！
    writev(1,iovector,2);
    return 0;
}