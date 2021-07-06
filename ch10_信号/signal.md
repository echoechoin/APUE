# 信号

[TOC]

## 10.2 信号的概念

需要注意的点有：

* 信号的默认动作
* 无法捕捉的信号SIGKILL、SIGSTOP(可以捕获SIGTSTOP在交互式命令行捕获ctrl+z)

常用的信号：

* SIGKILL (kill -9)
* SIGSTOP/SIGTSTOP(ctrl+z)
* SIGINT  (ctrl + c)
* SIGQUIT (ctrl + \)
* SIGABORT (abort函数)
* SIGALRM (alarm函数)

## 10.3 signal函数

1. signal函数的使用方法

    ```c
    void (*signal(int signo, void (*fun)(int)))(int)
    ```

    * 参数1：`int signo`
        表示要捕获的信号
    * 参数2：`void (*fun)(int)`

        表示捕获信号后的动作。
        也可以传入以下值：

        ```c
        # define SIG_ERR ((void)(*)())-1
        # define SIG_DFL ((void)(*)()) 0
        # define SIG_IGN ((void)(*)()) 1
        ```

    * 返回值：`void(*)(int)`
        返回指向信号处理函数的指针

2. 可以通过kill函数向进程发送信号:

    ```sh
    kill -USER1 ${pid}
    kill ${pid}    # 等价于kill -TERM ${pid}
    kill -9 ${pid} # 等价于kill -KILL ${pid} 
    ```

3. (我觉得比较)常用的信号
    信号 | 作用
    --|--
    `SIGSTOP` | 无法捕获的ctrl+z
    `SIGTSTP` | 可以捕捉的ctrl+z, 用于可交互的终端程序。
    `SIGQUIT` | ctrl + \
    `SIGINT`  | ctrl + c

## 10.4 不可靠信号

和可靠信号对比

## 10.5 中断的系统调用

进程在执行一个低速系统调用而阻塞期间捕捉到一个信号，则该系统调用就被中断不在执行。

该系统调用返回出错，其errno被置为`EINTR`。低速系统调用指的是可能会使进程永远阻塞的一类系统调用，包括：

* 读写某些文件
* 打开某些文件
* `pause`和`wait`函数
* 某些`ioctl`操作
* 某些进程间通信函数

## 10.6 可重入函数

大概就是被中断后保证不会出错的函数。

比如说`malloc`函数在执行的过程中被中断了，而中断处理函数中又调用了`malloc`，此时操作系统会保证前一个`malloc`不会出错。

## 10.8 可靠信号

对于Linux内核来说，可靠信号就是多个相同的信号触发过来时，信号会排队等待处理；不可靠信号不会排队处理，可能会导致后来的信号丢失。

> 早期的UNIX系统触发信号后会将不可靠信号恢复为默认动作。

## 10.9 kill和raise函数

函数                             | 作用
                              -- | --
int raise(signo);                | 向自己发送信号
int kill(pid_t pid, int signo);  | 将信号发送给进程或进程组

其中kill函数的参数1:

* pid > 0:  发送给进程号为pid的进程
* pid = 0:  发送给自己和自己所在的进程组
* pid < 0:  发送给进程组为|pid|的进程
* pid = -1: 发送给能发送的所有的进程

## 10.10 alarm和pause函数

unsigned int alarm(unsigned int seconds); // 每个进程只能有一个alarm，再次调用函数，表示重置alarm。定时结束后，产生SIGALRM信号，默认动作是终止进程。
int pause(void);

## 10.11 信号集(设置信号集的函数簇)

```c++
// sigpromask函数可以用到信号集
#include <signal.h>
int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
int sigaddset(sigset_t *set, int signum);
int sigdelset(sigset_t *set, int signum);
int sigismember(const sigset_t *set, int signum);
```

## 10.12 sigprocmask函数(仅用于单线程，阻塞、取消阻塞信号)

```c++
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
```

* `int how`
    SIG_BLOCK
    SIG_UNBLOCK
    SIG_SETMASK

* `const sigset_t *set`
    需要处理的信号集

* `sigset_t *oldset`
    处理之前的信号集

## 10.13 sigpending函数（获取当前未决信号集）

```c++
int sigpending(sigset_t *set);
```

## 10.14 sigaction函数（设置信号处理函数）

检查和修改与指定信号相关联的动作

```c++
#include <signal.h>
int sigaction(int signum, const struct sigaction *act,
                struct sigaction *oldact);

struct sigaction act;
act.sa_handler = handler;
act.sa_flags = 0; //表示处理信号时阻塞信号
act.mask = mask; // 表示处理信号时阻塞的信号

struct sigaction {
    void (*sa_handler)(int);
    void (*sa_sigaction)(int, siginfo_t *, void *);
    sigset_t sa_mask; // 调用信号处理函数之前阻塞某些信号, 信号处理函数结束后，恢复之前的mask
    int sa_flags;
    void (*sa_restorer)(void); // 没有被使用
}
```

signal函数的sa_flag和sa_mask等于0

## 10.15 sigsetjmp/siglongjmp（从信号中跳转出来）

sigsetjmp：当进入信号处理函数后，信号会被屏蔽，此时，想保留信号的屏蔽的话，就得在sigsetjump的时候，第二个参数大于0。如果等于0，跳出来后信号将不再屏蔽。

## 10.16 sigsuspend函数（阻塞等待某个信号）

mask用于阻塞信号，从而等待没有阻塞的信号唤醒sigsuspend。唤醒并从信号处理函数中返回后，换回原来的mask

```c++
int sigsuspend(const sigset_t *mask);
```

## 10.17 system函数

system函数忽略了SIGINT和SIGQUIT(此时ctrl+c 会发送到system函数处理的子程序中)，阻塞了SIGCHILD

```c++
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
    printf("sleep 10...\n");
    system("sleep 10"); // 此时ctrl + c 后SIGINT会被父进程忽略, 而子进程会处理SIGINT(sleep程序会默认处理SIGINT:退出程序)
    while(1);
    return 0;
}
```

## 10.18 sleep函数


