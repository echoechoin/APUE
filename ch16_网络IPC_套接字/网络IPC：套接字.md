# 网络IPC：套接字

[TOC]

## 16.2 套接字描述符

* 创建套接字

```c++
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
// domain: 确定通信的特性，包括地址格式。
//          AF_INET:  IPv4因特网域
//          AF_INET6: IPv6因特网域
//          AF_UNIX:  UNIX域
//          AF_UNSPEC:未指定 

// type: 确定套接字的类型，进一步确定通信特征
//      SOCK_DGRAM: 长度固定的、无连接的不可靠报文传递
//      SOCK_RAW:   IP协议的数据接口 （需要root权限）
//      SOCK_SEQPACKET: 长度固定、有序、可靠的面向连接报文传递
//      SOCK_STREAM: 有序、可靠、双向的面向连接字节流

// protocol 通常为0，表示按给定的域和套接字类型选择默认协议。当对同一域和套接字类型支持多个协议时，可以使用protocol参数选择一个特定协议。
```

* 关闭套接字

```c++
int shutdown(int sockfd, int how)
// how :
//      SHUT_RD
//      SHUT_WR
//      SHUT_RDWR
```

## 16.3 寻址

### 16.3.1 字节序

* 大端字节序： 最大字节地址对应数字最低有效字节上
* 小端字节序： 最小字节地址对应数字最低有效字节上

```c++
// 对于小端字节序存储0x04030201：
// 0x04 0x04
// 0x03 0x03
// 0x02 0x02
// 0x01 0x01 <---cp

// 对于大端字节序存储0x04030201：
// 0x04 0x01
// 0x03 0x02
// 0x02 0x03
// 0x01 0x04 <---cp

int i = 0x04030201;
char *cp = (char *)&i
```

* **TCP/IP协议采用大端字节序**

* 大小端字节序转换函数：

```c++
#include <arpa/inet.h>
#include <arpa/inet.h>

uint32_t htonl(uint32_t hostlong); // 主机字节序转网络字节序
uint16_t htons(uint16_t hostshort);
uint32_t ntohl(uint32_t netlong);
uint16_t ntohs(uint16_t netshort);


// af只支持AF_INET和AF_INET6
// 网络字节序转字符串
const char *inet_ntop(int af, const void *src,
                            char *dst, socklen_t size);
// 字符串转网络字节序
int inet_pton(int af, const char *src, void *dst);
```

## 16.3.2 地址转换

为使不同格式地址能够被传入到套接字函数中，地址被强行转换为通用的地址结构sockaddr表示（for linux）：

```c++
struct sockaddr {
    sa_family_t sa_family;
    char        da_data[14];
}

// IPv4套接字结构：
struct in_addr {
    in_addr_t s_addr;
}
struct sockaddr_in {
    sa_family_t    sa_family;
    in_port_t      sin_port; // uint16_t
    struct in_addr sin_addr; // uint32_t
    unsigned char  sin_zero[8]; // 必须全部置零
}

// IPv6:套接字将结构：
struct in6_addr {
    uint8_t s6_addr[16];
};
struct sockaddr_in6 {
    sa_family_t sin6_family;
    in_port_t sin6_port;
    uint32_t sin6_flowinfo;
    struct in6_addr sin6_addr;
    uint32_t sin6_scope;
}
```

### 16.3.3 地址查寻

### 16.3.4 将套接字与地址绑定

```c++
int bind(int sockfd, const struct sockaddr *addr,
        socklen_t addrlen);

// 查看套接字信息
int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

// 查看对方套接字信息
int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

```

### 16.4 建立连接

```c++
int connect(int sockfd, const struct sockaddr *addr,
            socklen_t addrlen);
```

* 面向连接的套接字使用connect连接服务器
* 面向无连接的套接字也可以使用connect函数，使用后，所有发送报文的目标地址设置为connect调用中指向的地址，且仅能接收来自指定地址的报文。

```c++
//宣告可以接收连接请求
int listen(int sockfd, int backlog);

// 等待连接
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

## 16.5 数据传输

* read和write可以收发网络套接字，但是功能不完全。

* 发送相关的函数

```c++
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
// flags:
//          MSG_DONTROUTE:  勿将数据路由出本地网络
//          MSG_DONTWAIT:   允许非阻塞操作(等价于使用O_NONBLOCK)
//          MSG_EOR:        协议支持表示结束连接
//          MSG_OOB:        协议支持表示发送带外数据
```

UDP协议最多send字节数：（65535 - 20（ip报文头部）-8（udp报文头部）

```c++
// 和send相似，但是可以为发送UDP报文指定一个套接字
// 对于面向连接的套接字，目标地址是被忽略的
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
                      const struct sockaddr *dest_addr, socklen_t addrlen);
```

```c++
ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
```

* 接收相关的函数

```c++
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
// flags:
//          MSG_OOB:        接收带外数据
//          MSG_PEEK:       返回报文内容而不是真正接收报文
//          MSG_TRUNC:      即使报文被截断也返回的是真实的报文长度
//          MSG_WAITALL:    等待直到所有的数据可用（仅TCP）（对于recv接收的数据可能比请求len的少）

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                struct sockaddr *src_addr, socklen_t *addrlen);

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);

```

## 16.6 套接字选项

```c++
int getsockopt(int sockfd, int level, int optname,
                    void *optval, socklen_t *optlen);
// level: 协议
//        对于通用选项：SOL_SOCKET，否则协议号：IPPROTO_TCP、IPPROTO_IP等

// val:   开启和关闭该选项

// len:   val的长度
int setsockopt(int sockfd, int level, int optname,
                    const void *optval, socklen_t optlen);
```

1. 通用选项，工作在所有套接字上
2. 套接字层次管理的选项
3. 特定某一个协议的选项

常用的选项：

1. 地址重用： `setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val))`

## 16.7 带外数据


