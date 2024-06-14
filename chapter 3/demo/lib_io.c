/*
 * @Author: Phyleoly Phyleoly@gmail.com
 * @Date: 2024-06-14 00:01:34
 * @LastEditTime: 2024-06-14 15:33:37
 * @Description: 实现 3.9 readn,writen,readline函数
 */
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

ssize_t readn(int filedes, void *buff, size_t nbytes);
ssize_t writen(int filedes, const void *buff, size_t nbytes);
ssize_t readline(int filedes, void *buff, size_t maxlen);

ssize_t readn(int filedes, void *buff, size_t nbytes) {
    size_t nleft = nbytes;
    ssize_t nread;
    char *ptr;
    ptr = buff;
    while (nleft > 0) {
        if ( (nread = read(filedes, ptr, nleft)) < 0) {
            if (errno == EINTR) 
                nread = 0;
            else 
                return -1;
        } else if (nread == 0)
            break;
        nleft -= nread;
        ptr += nread;
    }
    return (nbytes - nleft);
}

ssize_t writen(int filedes, const void *buff, size_t nbytes) {
    size_t nleft = nbytes;
    ssize_t nwrite;
    char *ptr;
    ptr = buff;
    while (nleft > 0) {
        if ( (nwrite = write(filedes, ptr, nleft)) <= 0) {
            if (nwrite < 0 && errno == EINTR)
                nwrite = 0;
            else
                return -1;
        }
        nleft -= nwrite;
        ptr += nwrite;
    }
    return nbytes;
}
