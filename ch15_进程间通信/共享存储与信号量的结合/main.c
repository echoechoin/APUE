#include <sys/ipc.h>
#include <sys/shm.h> // 共享存储
#include <sys/sem.h> // 信号量

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <errno.h>


int main(int argc, char *argv[]) {
    int pid = 0;
    int shmid = shmget(IPC_PRIVATE, 1024, IPC_CREAT);
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT);
    if (semid < 0){
        perror("semget error");
        printf("errno %d\n",errno);
    }
    
    if ((pid = fork()) < 0) {
        return 1;
    } else if (pid > 0) {
        char *addr = (char*)shmat(shmid, (const void*)0, 0);
        sleep(1);
        struct sembuf sops;
        sops.sem_flg = SEM_UNDO;
        sops.sem_num = 0;
        sops.sem_op = 1;
        if (semop(semid, &sops, 1) < 0){
            perror("semop error");
            return -1;
        }

        char *str = "hello world!";
        strcpy(addr, str);
        printf("addr: %p\n", addr);
        printf("str: %s\n", str);
        waitpid(pid, NULL, 0);
    } else {
        char *addr = shmat(shmid, (const void*)0, 0);

        struct sembuf sops;
        sops.sem_flg = SEM_UNDO;
        sops.sem_num = 0;
        sops.sem_op = -1;
        if (semop(semid, &sops, 1) < 0){
            perror("semop error");
            return -1;
        }

        char str[1024] = {0};
        strcpy(str,addr);
        printf("addr: %p\n", addr);
        printf("str: %s\n", str);
    }
    return 0;
}