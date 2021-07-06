# 高级IO

[TOC]

## 14.2 非阻塞IO（O_NONBLOCK）

指定非阻塞IO的两种方法：

* 在使用`open()`函数的时候指定`O_NONBLOCK`
* 对于已经打开的文件描述符，可以调用`fcntl`，由该函数打开`O_NONBLOCK`文件状态标志

以write函数为例

```c++
int fd = ...
int flags;
// 设置为非阻塞
if(flags = fcntl(fd, F_GETFL, 0) < 0)
{
    perror("fcntl");
    return -1;
}
flags |= O_NONBLOCK;
if(fcntl(fd, F_SETFL, flags) < 0)
{
    perror("fcntl");
    return -1;
}

while(ntowrite > 0)                   /* 这种形式的循环称为轮循，在多用户系统上它浪费了CPU时间。*/
{
    errno = 0;
    nwrite = write(STDOUT_FILENO, ptr, ntowrite);
    fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno); 
    // 写到标准输出的话，就可能会报错：errno = 11 EAGAIN：表示并没有写完，但是不会阻塞，所以返回了-1 errno置为EAGAIN
    // 因为标准输出的缓冲区无法一次性写入大量的数据

    if(nwrite > 0)
    {
        ptr += nwrite;
        ntowrite -= nwrite;
    }
}
```

## 14.3 记录锁（字节范围锁）(TODO: 还有一个文件锁叫做flock)

记录锁的功能就是，阻值其他进程修改同一文件区域（也可以是整个文件）。
> 两个不相干的进程也可以设置文件锁！
> TODO: lockf就是基于次函数实现的

```c++
// 设置文件的写锁
int fcntl(int fd, int cmd, ... /* arg */ );


// cmd: F_GETLK\F_SETLK\FSETLKW

// args: 
struct flock
{
    short int l_type;   /* 锁的类型: F_RDLCK, F_WRLCK, or F_UNLCK.*/
    short int l_whence; /* SEEK_SET, SEEKCUR, SEEKEND */
    __off_t l_start;    /* 起始偏移  */
    __off_t l_len;      /* 长度  */
    __pid_t l_pid;      /* 使用GETLK命令时可以查看占用锁的进程，SETLK(W)的时候不需要知道指定  */
};

```

1. 对于`flock`结构体：
    锁整个文件的方法： `l_whence = SEEK_SET; l_start = 0; l_len = 0(0表示最大的偏移量，新添加到文件的内容也会算在其中).`
    锁的类型：

    * `F_RDLCK`: 共享锁，多个进程有一个或多可读锁，则不能再有任何写锁
    * `F_WRLCK`：独占锁，如果有写锁，则不能再有任何读锁。
    * `f_UNLCK`: 表示取消锁
        **对于释放锁，以下操作也会释放锁**：
        * 终止进程
        * 关闭文件描述符(在同一个进程中关闭)：

            ```c++
            fd1 = open(name, ...)
            fd2 = open(name, ...)
            fd3 = dup(fd1);
            ...
            close(fd1); // 这样也会释放锁！！！
            close(fd3); // 这样也会释放锁！！！
            ```

2. 对于三个命令：
    * `F_GETLK`：测试能否获取一把锁
    * `F_SETLK`：非阻塞设置锁，失败返回-1
    * `F_SETLKW`：阻塞设置锁，知道能够获取到锁

3. 记录锁的继承

    * 对于`fork`的子进程不会继承记录锁
    * 执行`exec`后，新程序会继承原来的锁

4. 例子(非阻塞获取一个记录锁)

    ```c++
    int fd = ...;
    struct flock flk;
    flk.l_type = F_WRLCK;
    flk.l_whence = SEEK_SET;
    flk.l_start = 0; // 锁整个文件
    flk.l_len = 0;

    if (fcntl(fd, F_SETLKW, &flk) < 0) {
        perror("fcntl error");
        exit(errno);
    }
    ```

5. 强制性锁

## 14.4 STREAMS

与字符设备相关的函数

## 14.5 I/O多路转接

> 不论描述符是否阻塞，都不影响select和poll是否阻塞  

### select函数（pselect与select类似）

> * 三个集合设置关心的文件描述符
> * 时间参数设置select等待的时间，超时就会返回，也可以设置为阻塞：NULL

```c++
int select(int nfds, fd_set *readfds, fd_set *writefds,
            fd_set *exceptfds, struct timeval *timeout);
```

* `timeout`
    愿意等待的时间，超时select返回。注意，每次调用select函数后需要重新设置此参数，因为被此函数修改了。

    ```c++
    struct timeval {
        long    tv_sec;         /* seconds */
        long    tv_usec;        /* microseconds */
    };
    // timeout = NULL 表示阻塞
    // timeout = {0, 0}表示立即返回
    // timeout = {x, x}表示设置需要等待的时间
    ```

* `fd_set *readfds, fd_set *writefds, fd_set *exceptfds`
    指向三个文件描述符集, 表示可读、可写、异常状态。使用宏函数控制：

    ```c++
    int FD_ISSET(int fd, fd_set *fdset);
    void FD_CLR(int fd, fd_set *fdset);
    void FD_SET(int fd, fd_set *fdset);
    void FD_ZERO(fd_set *fdset);
    ```

    某个参数为NULL，表示我们不关心是否可读、可写、异常；三个参数都是NULL，该函数就变成了精确定时的sleep函数了（哈哈哈）

* nfds
    最大的描述符+1(可以设置为FD_SETSIZE 1024)

* 返回值
    -1：出错
     0：没有描述符准备好
     1：已经准备好的描述符

* 例子

    ```c++
    #include <stdio.h>
    #include <unistd.h>
    #include <sys/select.h>
    #include <errno.h>
    #include <stdlib.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>

    int main () {
        int file1_fd = open("./file1", O_RDWR);
        if (file1_fd < 0) {
            perror("open file1 error");
            exit(errno);
        }
        fd_set rd_set, wt_set, err_set;
        struct timeval tv;
        while (1) {
            // 每次需要重新设置关心的行为！
            FD_ZERO(&rd_set);
            FD_ZERO(&wt_set);
            FD_ZERO(&err_set);
            // FD_SET(file1_fd, &rd_set);
            // FD_SET(file1_fd, &wt_set);
            // FD_SET(file1_fd, &err_set);

            // 每次需要重新设置超时时间
            tv.tv_sec = 5;
            tv.tv_usec = 0;
            int nums = select(file1_fd+1, &rd_set, &wt_set, &err_set, &tv);
            if (nums < 0) {
                perror("select error");
                exit(errno);
            }
            if (nums == 0) {
                printf("no fd is prepared.\n");
            }
            if (FD_ISSET(file1_fd, &rd_set)) {
                printf("file1 is readable.\n");
            } if (FD_ISSET(file1_fd, &wt_set)) {
                printf("file1 is writeable.\n");
            } if (FD_ISSET(file1_fd, &err_set)) {
                printf("file1 fd has error.\n");
            }

            // timeout被修改了！
            printf("sec:  %d\n", tv.tv_sec);
            printf("usec: %d\n", tv.tv_usec);
        }

        return 0;
    }
    ```

### poll

```c++
#include <poll.h>
int poll(struct pollfd *fds, nfds_t nfds, int timeout);

struct poolfd {
    int fd;
    short events; // 关注的事件
    short revents; // 发生的事件
};

// 对于events
#define POLLIN      0x001       /* There is data to read.  */
#define POLLRDNORM  0x040       /* Normal data may be read.  */
#define POLLRDBAND  0x080       /* Priority data may be read.  */
#define POLLPRI     0x002       /* There is urgent data to read.  */

#define POLLOUT     0x004       /* Writing now will not block.  */
#define POLLWRNORM  0x100       /* Writing now will not block.  */ //和POLLOUT一样
#define POLLWRBAND  0x200       /* Priority data may be written.  */

#define POLLERR     0x008       /* Error condition.  */ 
#define POLLHUP     0x010       /* Hung up.  */ // 已挂断，只可读，不可写
#define POLLNVAL    0x020       /* Invalid polling request.  */

// 对于timeout，表示poll阻塞的时间，-1表示永远等待，0表示立即返回，大于0表示等待的时间
```

* `struct pollfd *fds`
等待监听的一个数组

* `nfds_t nfds`
监听数组的大小

* `int timeout`
超时

## 14.6 异步IO

使用信号通知程序io是否可以读写

## 14.7 readv和writev

读写多个非连续缓冲区（散步读、聚集写）

```c++
#include <sys/uio.h>

ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
ssize_t writev(int fd, const struct iovec *iov, int iovcnt);

struct iovec {
    void *iov_base; // 表示缓冲区
    size_t iov_len; // 表示长度
};

```

* `int fd`
* `const struct iovec *iov`
* `int iovcnt`
iov数组的大小。最多`IOVMAX`个

## 14.8 readn和writen函数

只有读取或写入了n个字节后才返回

linux没有封装readn和writen

## 14.9 存储映射io

存储映射I/O使用一个磁盘文件与存储空间中的一个缓冲区相映射。于是当从缓冲区取数据，就相当于读文件中的响应字节。与此类似，将数据存入缓冲区，则相应字节就自动写入文件，这样可以不用read和write的情况下执行IO。

```c++
#include <sys/mman.h>

void *mmap(void *addr, size_t length, int prot, int flags,
            int fd, off_t offset);

```

* `int prot`

以下选项可以与或

prot | 说明
-|-
PROT_READ  | 映射区可读
PROT_WRITE | 映射区可写
PROT_EXEC  | 映射区可执行
PROT_NONE  | 映射区不可访问

* `void *addr`和`off_t offset`
    无脑0就完事了
* int flags
    `MAP_FIXED` 强制将映射后的地址设置为addr， 不建议。如果没有设置此flag, addr非0表示建议操作系统设置为此地址。
    `MAP_SHARED` 指定存储操作修改映射文件
    `MAP_PRIVATE` 文件的副本，修改此存储区不会修改到真实的文件
    `MAP_SHARED`和`MAP_PRIVATE`只能指定一个
    `MAP_ANON` 匿名映射，设置匿名映射时，fd = -1

```c++
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
main(){
    int fd;
    void *start;
    struct stat sb;
    fd = open("/etc/passwd", O_RDONLY); /*打开/etc/passwd */
    fstat(fd, &sb); /* 取得文件大小 */
    start = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(start == MAP_FAILED) /* 判断是否映射成功 */
        return;
    printf("%s", start); munma(start, sb.st_size); /* 解除映射 */
    closed(fd);
}
```
