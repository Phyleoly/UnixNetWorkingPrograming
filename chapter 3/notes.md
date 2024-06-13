### 3.1 概述
套接字可以在两个方向上传递：从进程到内核，从内核到进程

### 3.2 套接字地址结构
#### 3.2.1 IPV4 套接字地址结构
又称“网络套接字地址结构”，以`sockaddr_in`命名，定义于`<netinet/in.h>`中
~~~C++
struct in_addr {
    in_addr_t s_addr;
};

struct sockaddr_in {
    uint8_t sin_len;
    sa_family_t sin_family;
    in_port_t sin_port

    struct in_addr sin_addr;

    char sin_zero[8];
};
~~~

#### 3.2.2 通用套接字地址结构
套接字函数必须支持来自任何协议族的套接字地址结构指针作为参数，这一问题在ANSI C后解决起来很简单（使用`void *`通用指针类型），但套接字函数定义于ANSI C前，于是1982年采用定义一个通用的套接字地址结构`sockaddr`，位于`<sys/sock.h>`中
~~~C++
struct sockaddr {
    uint8_t sa_len;
    sa_family_t sa_family;
    char sa_data[14];
}
~~~

#### 3.2.3 IPV6 套接字地址结构
IPV6 套接字地址结构在`<netinet/in.h>`中定义
~~~C++
struct in6_addr {
    uint8_t sa_addr[16];

};
#define SIN6_LEN
struct sockaddr_in6 {
    uint8_t sin6_len;
    sa_family_t sin6_family;
    in_port_t sin6_port;

    uint32_t sin6_flowinfo;
    struct in6_addr sin6_addr;

    uint32_t sin6_scope_id;
}
~~~

#### 3.2.4 新的通用套接字地址结构
`sockaddr_storage`在`<netinet/in.h>`中定义

### 3.3 值-结果参数
- 从进程到内核传递套接字地址结构的函数有3个：`bind`、`connect`、`sendto`，这些函数一个参数是指向某个套接字的指针，另一个参数则是该结构的整数大小，传给内核后，内核便知道从进程复制多少数据进来。
- 从内核到进程传递套接字地址结构的函数有4个：`accept`、`recvfrom`、`getsockname`、`getpeername`，这些函数一个参数是指向某个套接字的指针，另一个则是指向表明该结构大小的整数变量的**指针**。这种类型的参数称为值-结果（value-result）参数。

### 3.4 字节排序函数
小端（little-endian）法就是低位字节排放在内存的低地址端即该值的起始地址，高位字节排放在内存的高地址端。
大端（big-endian）法就是高位字节排放在内存的低地址端即该值的起始地址，低位字节排放在内存的高地址端。
~~~c
/* byteorder.c : 判断主机字节序 */
#include <stdio.h>

int main(int argc, char **argv) 
{
    union {
        short s;
        char c[sizeof(short)];
    } un;
    un.s = 0x0102;
    
    if (sizeof(short) == 2) {
        if(un.c[0] == 1 && un.c[1] == 2)
            printf("big-endian\n");
        else if(un.c[0] == 2 && un.c[1] == 1)
            printf("little-endian");
        else 
            printf("unknown");
    } else
        printf("size(short) = %d\n", (int)sizeof(short));

    return 0;
}
~~~
网络协议使用大端字节序来传送多字节整数，主机字节序和网络字节序之间相互转换使用以下4个函数
~~~C
#include <netinet/in.h>
uint16_t htons(uint16_t host16bitvalue);
uint32_t htonl(uint32_t host32bitvalue);        //均返回网络字节序的值
uint16_t ntohs(uint16_t net16bitvalue);
uint32_t ntohl(uint32_t net32bitvaule);         //均返回主机字节序的值
~~~

### 3.5 字节操纵函数
~~~C
//4.2 BSD
#include <strings.h>
void bzero(void *dest, size_t nbytes);
void bcopy(const void *src, void *dest, size_t nbytes);
int bcmp(const void *ptrl, const void *ptr2, size_t nbytes);        //返回：若相等则为0，否则为非0
~~~
~~~C
//ANSI C
#include <string.h>
void *memset(void *dest, int c, size_t len);
void *memcpy(void *dest, const void *src, size_t nbytes);
int memcmp(const void *ptr1, const void *ptr2, size_t nbytes);      //返回：若相等则为0，否则为<0或>0

//针对源字节串与目标字节串重叠时，bcopy能正确处理，memcpy结果不确定
//需改用ANSI C的memmove函数
~~~

### 3.6 inet_aton、inet_addr和inet_ntoa函数
这些函数在点分十进制数串（如：“206.168.112.96”）与它长度为32位的网络字节序二进制值间转换IPv4地址
~~~C
#include <arpa/inet.h>

int inet_aton(const char *strptr, struct in_addr *addrptr);     //返回：若字符串有效则为1，否则为0
in_addr_t inet_addr(const char *strptr);        
        //返回：若字符串有效则为32位二进制网络字节序的IPv4地址，否则为INADDR_NONE;
char *inet_ntoa(struct in_addr inadddr);                        //返回：指向一个点分十进制数串的指针
~~~

### 3.7 inet_pton和inet_ntop函数
p代表表达（presentation），n代表数值（numeric）
~~~C
#include <arpa/inet.h>
int inet_pton(int family, cnost char *strptr, void *addrptr);
        //返回：若成功则为1，若输入不是有效的表达格式则为0，若出错则为-1
const char *inet_ntop(int family, const void *addrptr, char *strptr, size_t len);
        //返回：若成功则为指向结果的指针，若出错则为NULL
~~~
如果以不被支持的地址族作为family参数，这两个函数都返回一个错误，并将errno置为EAFNOSUPPORT。
对于`inet_ntop`，如果len过小，返回一个空指针，并将errno职位ENOSPC。

### 3.8 sock_ntop和相关函数
为解决使用inet_ntop时会使代码和协议相关，自行编写一个名为sock_ntop的函数，它以指向某个套接字地址结构的指针为参数.
~~~C
char *sock_ntop(const struct sockaddr *sockaddr, socklen_t addrlen) {
        //返回：若成功则为非空指针，若出错则为NULL
    static char str[128];  // 使用静态变量存储结果，避免返回局部变量的指针
    char portstr[8];

    switch (sockaddr->sa_family) {
    case AF_INET: {
        const struct sockaddr_in *sa_in = (const struct sockaddr_in *)sockaddr;

        if (inet_ntop(AF_INET, &(sa_in->sin_addr), str, sizeof(str)) == NULL)
            return NULL;
        if (ntohs(sa_in->sin_port) != 0) {
            snprintf(portstr, sizeof(portstr), ":%d", ntohs(sa_in->sin_port));
            strcat(str, portstr);
        }
        return str;
    }
    case AF_INET6: {
        const struct sockaddr_in6 *sa_in6 = (const struct sockaddr_in6 *)sockaddr;

        str[0] = '[';
        if (inet_ntop(AF_INET6, &(sa_in6->sin6_addr), str + 1, sizeof(str) - 1) == NULL)
            return NULL;
        if (ntohs(sa_in6->sin6_port) != 0) {
            snprintf(portstr, sizeof(portstr), "]:%d", ntohs(sa_in6->sin6_port));
            strcat(str, portstr);
            return str;
        }
        return str + 1;
    }
    default:
        return NULL;
    }
}
~~~

### 3.9 readn、writen和readline函数
字节流套接字上的read和write函数输入或输出的字节数可能比请求的数量少，这个现象的原因在于内核中用于套接字的缓冲区可能已达到极限。此时需要调用者再次调用read和write函数，但是在写一个字节流套接字时说只能在该套接字为非阻塞的前提下实现，所以改为调用writen函数来取代write函数
~~~C
ssize_t readn(int filedes, void *buff, size_t nbytes);
ssize_t writen(int filedes, const void *buff, size_t nbytes);
ssize_t readline(int filedes, void *buff, size_t maxlen);
        //均返回：读或写的字节数，若出错则为-1
~~~