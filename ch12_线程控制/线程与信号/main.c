#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void wait_SIGUSR1() {
    int num;
    sigset_t set, oldset;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    pthread_sigmask(SIG_BLOCK, &set, &oldset); // 在调用sigwait前阻塞需要wait的信号
    sigwait(&set, &num);
    pthread_sigmask(SIG_SETMASK, &oldset, NULL); // 还原之前的信号集
}

void *thread(void *args) {
    wait_SIGUSR1();
    printf("SIGUSER1 is comming...\n");
    return (void*)0;
}

int  main() {
    sigset_t set;
    sigfillset(&set);
    sigdelset(&set, SIGQUIT);
    pthread_sigmask(SIG_SETMASK, &set, NULL); // 主线程阻塞所有信号

    pthread_t tid;
    pthread_create(&tid, NULL, thread, NULL);
    pthread_join(tid, NULL);    
    return 0;
}