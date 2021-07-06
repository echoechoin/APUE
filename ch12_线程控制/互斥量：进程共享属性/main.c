#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define BUF_SIZE 1024

int main () {
    // 设置为进程共享
    pthread_mutexattr_t mutexattr;
    pthread_mutexattr_init(&mutexattr);
    pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED);

    // mmap映射内存并分配锁
    pthread_mutex_t *mutex = (pthread_mutex_t *)mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    pthread_mutex_init(mutex, &mutexattr);

    // 进程之间使用互斥锁
    int pid = fork();
    if (pid > 0) {
        while (1) {
            while (pthread_mutex_trylock(mutex) != 0) {
                printf("trylock\n");
            }
            printf("porcess1\n");
            pthread_mutex_unlock(mutex);
            sleep(1);
        }
    } else if (pid == 0) {
        while (1) {
            while (pthread_mutex_trylock(mutex) != 0) {
                printf("trylock\n");
            }
            printf("porcess2\n");
            pthread_mutex_unlock(mutex);
            sleep(1);
        }
    }

    return 0;
}
