#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

void * thread(void * args)
{
    int ret; 
    int fd = open("./main.lock", O_RDWR | O_CREAT);
    struct flock lock = {
        .l_len = 0,
        .l_type = F_RDLCK,
        .l_whence = 0,
        .l_start = SEEK_SET
    };
    if ((ret =fcntl(fd, F_SETLK, &lock)) != 0 ) {
        printf("get lock failed\n");
    }
    else {
        printf("get lock success\n");
    }
    return 0;
}

int main (int argc, char *argv[])
{
    pthread_t tid[2];
    // 两个线程都能获取记录锁
    pthread_create(&tid[0], NULL, thread,NULL);
    pthread_create(&tid[1], NULL, thread,NULL);

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    return 0;
}