#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    key_t key = ftok("./semipc", 'z');
    printf("key: %d\n", key);
    if (key < 0) {
        perror("key");
        exit(0);
    }
    int semid = semget(key, 0, IPC_CREAT);
    if (semid < 0) {
        perror("semid");
        exit(0);
    }
    union semun {
		int		val;
		struct semid_ds	*buf;
		unsigned short		*array;
	} semctl_arg;
    semctl_arg.val = 10;
    if (0 > semctl(semid, 0, SETVAL,semctl_arg)) {
        perror("semctl");
        exit(0);
    }

    printf("nums: ");

    struct sembuf sops;
    sops.sem_flg = 0;
    sops.sem_num = 0;
    sops.sem_op = -1;
    if (semop(semid,&sops, 1) < 0) {
        perror("semop");
        exit(0);
    }
    return 0;
}