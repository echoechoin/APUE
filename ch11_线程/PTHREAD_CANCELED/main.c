#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

void *thread(void * args) {
    pthread_exit(PTHREAD_CANCELED);
}

int main ()
{
    
    return 0;
}