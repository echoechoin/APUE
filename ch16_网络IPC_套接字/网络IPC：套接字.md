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

```c++
struct hostent *gethostent(void);
void sethostent(int stayopen);
void endhostent(void);
```
