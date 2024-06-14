#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

//todo:

char *sock_ntop(const struct sockaddr *sockaddr, socklen_t addrlen);

//将通配地址和一个临时端口捆绑到一个套接字
//返回：若成功则为0，若出错则为-1
int sock_bind_wild(int sockfd, int family);

//比较两个套接字地址结构的地址部分
//返回：若地址为统一协议族且相同则为0，否则为非0
int sock_cmp_addr(const struct sockaddr *sockaddr1, const struct sockaddr *sockaddr2, socklen_t addrlen);

//比较两个套接字地址结构的端口部分
//返回：若地址为统一协议族且端口相同则为0，否则为非0
int sock_cmp_port(const struct sockaddr *sockaddr1, const struct sockaddr *sockaddr2, socklen_t addrlen);

//把一个套接字地址结构中的主机部分转换成表达格式（不包括端口号）
//返回：若成功则为非空指针，若出错则为-1
char *sock_ntop_host(const struct sockaddr *sockaddr, socklen_t addrlen);

void sock_set_addr(const struct sockaddr *sockaddr, socklen_t addrlen, void *ptr);
void sock_set_port(const struct sockaddr *sockaddr, socklen_t addrlen, int port);
//把一个套接字地址结构中的地址部分置为通配地址
void sock_set_wild(const struct sockaddr *sockaddr, socklen_t addrlen);

char *sock_ntop(const struct sockaddr *sockaddr, socklen_t addrlen) {
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
