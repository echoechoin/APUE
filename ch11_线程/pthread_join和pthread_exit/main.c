#include <pthread.h>
#include <stdio.h>

void *thread1_callback(void *args) {
    printf("thread1 exit\n");
    pthread_exit((void*)1);
}

void *thread2_callback(void *args) {
    printf("thread2 return\n");
    return ((void*)1);
}

int main() {
    pthread_t tid1, tid2;
    int err = 0;
    void *thread_retval;

    pthread_create(&tid1, NULL, thread1_callback, NULL);
    if (err < 0) {
        perror("thread1 create");
        return -1;
    }

    pthread_create(&tid2, NULL, thread2_callback, NULL);
    if (err < 0) {
        perror("thread2 create");
        return -1;
    }

    err = pthread_join(tid1, &thread_retval);
    if (err < 0) {
        perror("thread1 join");
        return -1;
    }
    printf("thread1 ret: %d\n", (int)(thread_retval));

    err = pthread_join(tid2, &thread_retval);
    if (err < 0) {
        perror("thread2 join");
        return -1;
    }
    printf("thread2 ret: %d\n", (int)(thread_retval));

    return 0;
}