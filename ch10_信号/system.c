#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>

void docatch(int signo)
{
    printf("%d signal is catch\n", signo);
}

int main()
{
    int ret;
    struct sigaction act;
    act.sa_handler = docatch;
    sigemptyset(&act.sa_mask);
    // sigaddset(&act.sa_mask, SIGQUIT);
    act.sa_flags = 0;  //默认属性：信号捕捉函数执行期间，自动屏蔽本信号
    ret = sigaction(SIGINT, &act, NULL);
    if(ret < 0)
    {
        perror("sigaction error");
        exit(1);
    }
    printf("./main\n");
    system("sleep 10"); // 此时ctrl + c 后SIGINT会被父进程忽略, 而子进程会处理SIGINT
    while(1);
    return 0;
}