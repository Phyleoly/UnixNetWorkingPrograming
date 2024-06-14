<!--
 * @Author: Phyleoly Phyleoly@gmail.com
 * @Date: 2024-06-14 16:17:41
 * @LastEditTime: 2024-06-14 18:06:24
 * @Description: 
 * 
 * Copyright (c) 2024 by Phyeloly, All Rights Reserved. 
-->
### 4.2 socket 函数
~~~C
#include <sys/socket.h>
int socket(int family, int type, int protocol);
        //返回：若成功则为非负描述符，若出错则为-1
~~~
`socket`函数在成功时返回一个小的非负整数值，它与文件描述符类似，被为套接字描述符（socket descriptor），简称sockfd

### 4.3 connect 函数
TCP客户用`connect`函数来建立与TCP服务器的连接。
~~~C
#include <sys/socket.h>
int connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen)
        //返回：若成功则为0，若出错则为-1
~~~
客户在调用`connect`前不必非得调用`bind`，如果需要的话，内核会确定源IP地址，并选择一个临时端口作为源端口
`connect`函数导致当前套接字从CLOSED状态（套接字从`socket`创建以来一直所处的状态）转移到SYN_SENT状态，若成功则再转移到ESTABLISHED状态，失败则套接字不再可用，必须关闭，不能对这样一个套接字再次调用`connect`。

### 4.4 bind 函数
`bind`函数把一个本地协议地址赋予一个套接字。
~~~C
#include <sys/socket.h>
int bind(int sockfd, const struct sockaddr *myaddr, socklen_t adddrlen)
        //返回：若成功则为0，若出错则为-1
~~~
- 如果一个TCP客户端或服务器未曾调用`bind`捆绑一个端口，当调用`connect`或`listen`时，内核就要为响应的套接字选择一个临时端口。对于客户端来说这一行为很常见，对服务器来说则极为罕见。
- 进程可以把一个特定IP地址捆绑到它的套接字上。对于TCP客户，这就为在套接字上发送的IP数据报指派了源IP地址。对于TCP服务器，这就限定该套接字只接收哪些目的地为这个IP地址的客户连接。
- 如果让内核为套接字选择一个临时端口号，`bind`并不返回所选择的值，`bind`的第二个参数有`const`限定词，它无法返回所选之值，为了得到这个值，需调用`getsockname`返回地址协议。

### 4.5 listen 函数
`listen`函数仅由TCP服务器调用，它把一个未连接的套接字转换成一个被动套接字，只是内核应该接受该套接字的连接请求。调用`listen`会导致该套接字从CLOSED状态转换成LISTEN状态。
~~~C
#include <sys/socket.h>
int listen(int sockfd, int backlog);
        //返回，若成功则为0，若出错则为-1
~~~

### 4.6 accept 函数
`accept`函数由TCP服务器调用，用于从已完成连接队列队头返回下一个已完成连接。如果已完成连接队列为空，那么进程被置于休眠状态（假定套接字为默认的阻塞方式）。
~~~C
#include <sys/socket.h>
int accept(int sockfd, struct sockaddr *cliaddr, socklen_t addrlen);
        //返回，若成功则为非负描述符，若出错则为-1
~~~
`addrlen`是值-结果参数：调用前需将`*addrlen`所引用的整数值置为由`cliaddr`所指的套接字地址结构的长度，返回时，该整数值即为由内核存放在该套接字地址结构内的确切字节数。

### 4.7 fork 和 exec 函数
`fork`是Unix中派生新进程的唯一方法。
~~~C
#include <unistd.h>
pid_t fork(void);       //返回：在子进程中为0，在父进程中为子进程ID，若出错则为-1
~~~

`exec`把当前进程映像替换成新的程序文件，而且该程序通常从`main`函数开始执行。进程ID并不改变。我们称调用`exec`的进程调用进程（calling process），称新执行的程序为新程序（new program）。

### 4.8 并发服务器
Unix中编写并发服务器程序最简单的方法就是`fork`一个子进程来服务每个客户。
~~~C
//并发服务器程序轮廓
pid_t pid;
int listenfd, connfd;
listenfd = Socket( ... );
Bind(listenfd, ... );
Listen(listenfd, LISTENQ);
for ( ; ; ) {
    connfd = Accept(listenfd, ... );
    if ( (pid = Fork()) == 0) {
        Close(listenfd);
        doit(connfd);
        Close(connfd);
        exit(0);
    }
    Close(connfd);
}
~~~
这里需要多次调用`Close`的原因是，创建子进程后相应套接字的引用计数会增加。

### 4.9 close函数
`close`函数也用来关闭套接字，并终止TCP连接。
~~~C
#include <unistd.h>
int close(int sockfd);      //返回：若成功则为0，若出错则为-1
~~~
如果我们想在某个TCP连接上发送一个FIN，那么可以改用`shutdown`。

### 4.10 getsockname 和 getpeername 函数
`getsockname`返回与某个套接字关联的本地协议地址，`getpeername`返回与某个套接字关联的外地协议地址。
~~~C
#include <sys/sock.h>
int getsockname(int sockfd, struct sockaddr *localaddr, socklen_t *addr_len);
int getpeername(int sockfd, struct sockaddr *peeraddr, socklen_t *addr_len);
        //返回：若成功则为0，若出错则为-1
~~~
