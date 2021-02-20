#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>



#define MAXLINE 8192
//#define SERV_PORT 8000


#define SERV_PORT 8080


int main(int argc, char *argv[])
{
    struct sockaddr_in servaddr;
    char buf[MAXLINE];
    int sockfd, n;
    int i=20;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
   // inet_pton(AF_INET, "121.5.140.194", &servaddr.sin_addr);
   inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
    servaddr.sin_port = htons(SERV_PORT);

    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    while (1) {
        sprintf(buf,"%d",i);
        write(sockfd, buf, strlen(buf));
        printf("write success ");
        sleep(3);
        i++;
        //n = read(sockfd, buf, MAXLINE);
        // if (n == 0) {
        //     printf("the other side has been closed.\n");
        //     break;
        // }
        // else
        //     write(STDOUT_FILENO, buf, n);
    }
    close(sockfd);

    return 0;
}

