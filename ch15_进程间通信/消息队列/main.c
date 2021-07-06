#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

struct msg_t
{
    long msgtype;
    char data[512];
};


int main() {
    
    int pid = fork();
    if (pid > 0) {
        key_t key = ftok("./messageq", 'b');
        int msgqid = msgget(key, IPC_CREAT);
        struct msg_t msg = {
            .msgtype = 1,
            .data = "hello world!",
        };
        sleep(2);
        printf("send: %s\n", msg.data);
        msgsnd(msgqid, (void*)&msg, sizeof(msg)- sizeof(long), 0); // 发出去的消息会永远保存到内核中？
        waitpid(pid, NULL, 0);
        exit(0);
    } else if (pid == 0) {
        key_t key = ftok("./messageq", 'b');
        int msgqid = msgget(key, IPC_CREAT);
        if ( msgqid < 0) {
            perror("msgget");
        }
        struct msg_t msg = {0};

        if(0 > msgrcv(msgqid, (void *)&msg, sizeof(msg) - sizeof(long), 1, IPC_NOWAIT)) {
            perror("msgrcv");
        } else {
            printf("recv: %s\n", msg.data);
        }
        exit(0);
    } else {
        perror("fork");
        exit(1);
    }
}