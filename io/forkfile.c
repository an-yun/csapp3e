//
// Created by zuo on 2026/3/8.
// 父进程和子进程交叉读取
//

#include "csapp.h"

int main() {
    int fd;
    char c;
    fd = Open("foobar.txt", O_RDONLY, 0);
    if (Fork() == 0) {
        Read(fd, &c, 1);
        printf("child c = %c\n", c);
        Sleep(2);
        Read(fd, &c, 1);
        printf("child c = %c\n", c);
        exit(0);
    }
    Sleep(1);
    Read(fd, &c, 1);
    printf("parent c = %c\n", c);
    Wait(NULL);
    Read(fd, &c, 1);
    printf("parent c = %c\n", c);
    exit(0);
}
