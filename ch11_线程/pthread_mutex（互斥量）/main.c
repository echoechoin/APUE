#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
typedef struct {
    int i;
    pthread_mutex_t lock;
} resource_t;

void *thread(void *args) {
    int ret;
    resource_t *resource = (resource_t *)args;
    while(1) {
        if (0 != (ret = pthread_mutex_trylock(&resource->lock))) {
            printf("try lock return: %d\n",ret);
        } else {
            break;
        }
    }
    resource->i++;
    printf("i = %d\n", resource->i);
    pthread_mutex_unlock(&resource->lock);
    return (void *)0;
}

int main () {
    resource_t resource;
    resource.i = 1;
    
    printf("i = %d\n", resource.i);
    pthread_t tid1, tid2;
    pthread_mutex_init(&resource.lock, NULL);
    pthread_create(&tid1,NULL, thread, (void*)&resource);
    pthread_create(&tid2,NULL, thread,(void*)&resource);
    pthread_join(tid1,NULL);
    pthread_join(tid1,NULL);
    return 0;
}