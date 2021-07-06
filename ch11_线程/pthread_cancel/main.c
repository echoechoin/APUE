#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
void * thread1(void * args)
{
    for(;;){
        printf("hahah\n"); // 系统调用时会检查是否cancel了 
    }
    return 0;
}

void * thread2(void * args)
{
    for(;;){
        // 没有系统调用不会退出此线程
    }
    return 0;
}

int main ()
{
    pthread_t tid;
    pthread_create(&tid, NULL, thread2, NULL);
    pthread_cancel(tid);
    if (0 > pthread_join(tid, NULL)) {
        perror("pthread_join:");
    }
    return 0;
}