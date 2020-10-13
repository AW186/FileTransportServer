#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include "Sockets.hpp"
#include <string.h>
#include <stdlib.h>
#define UPLOAD "UPLD"
#define DOWNLOAD "DNLD"
#define SUCCESS "SUCCEED"
#define FAILED "FAIL"

char *readline(int fd, int maxLen) {
    char c;
    char *buff = (char *)malloc(maxLen);
    bzero(buff, maxLen);
    for (int index = 0; read(fd, &c, 1) == 1 && index < maxLen; index++) {
        if (c == '\r') {
            if (read(fd, &c, 1)) {
                if (c == '\n') {
                    break;
                } else {
                    buff[index++] = '\r';
                    buff[index] = c;
                }
            } else {
                buff[index] = '\r';
                break;
            }
        }
        buff[index] = c;
    }
    return buff;
}
int upload(int sockfd, const char *path) {
    int filefd = open(path, O_CREAT | O_TRUNC | O_WRONLY);
    char *buff = (char *)malloc(1025);
    bzero(buff, 1025);
    ssize_t size;
    printf("executing upload\n");
    while((size = read(sockfd, buff, 1024)) > 0) {
        printf("size: %zu\n", write(filefd, buff, size));
        bzero(buff, 1024);
    }
    if (!size) {
        printf("Ended at zero\n");
    }
    if (size < 0) {
        perror("read: ");
    }
    return 0;
}
int download(int sockfd, const char *path) {
    int filefd = open(path, O_RDONLY);
    char *buff = (char *)malloc(1025);
    bzero(buff, 1025);
    ssize_t size;
    printf("executing download\n");
    while((size = read(filefd, buff, 1024)) > 0) {
        printf("size: %zul\n", size);
        write(sockfd, buff, size);
        bzero(buff, 1024);
    }
    if (!size) {
        printf("Ended at zero\n");
    }
    if (size < 0) {
        perror("read: ");
    }
        return 0;
}
int main(int argc, char **argv) {
        auto pf = [](void *captured, int sockfd, struct sockaddr saddr, socklen_t socklen){
            char *command = readline(sockfd, 20);
            char *path = readline(sockfd, 1024);
            printf("command: %s\npath: %s\n", command, path);
            if (!strcmp(command, UPLOAD)) {
                upload(sockfd, path);
            } else if (!strcmp(command, DOWNLOAD)) {
                download(sockfd, path);
            }
            close(sockfd);
            printf("End session\n");
        };
        return server(8001, NULL, pf);
}
