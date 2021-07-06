#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void clean (void *args){
    printf("thread clean\n");
}

void *thread1 (void * args) {
    pthread_cleanup_push(clean, NULL);

    pthread_cleanup_pop(1);
    return (void*)0;
}

void *thread2 (void * args) {
    pthread_cleanup_push(clean, NULL);


    pthread_exit((void*)0);
    pthread_cleanup_pop(1);
}

int main () {
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, thread1, NULL);
    pthread_create(&tid2, NULL, thread2, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    return 0;
}