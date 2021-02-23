#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "json/json.h"
#include<iostream>
#include<string>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <ctime>

#define MAXLINE 8192
//#define SERV_PORT 8000
#define SERV_PORT 8080


int main(int argc, char *argv[])
{
    struct sockaddr_in servaddr;
    char buf[MAXLINE];
    int sockfd, n;
    int i=20;

    Json::Value root;
    root["user"] = 1;
    int code = 0;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
   // inet_pton(AF_INET, "121.5.140.194", &servaddr.sin_addr);
   inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
    servaddr.sin_port = htons(SERV_PORT);
   connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    // while (1) {
    //     root.toStyledString();
    //     std::string send_buff = root.toStyledString();
	//     char str2[2000];	
	//     strcpy(str2,send_buff.c_str());
    //     std::cout<<"send success"<<std::endl;

    //     write(sockfd, str2, strlen(str2));

    //     sleep(5);

    // }
    int FirstTime = 0;
    int SecondTime = 0;

    //while (fgets(buf, MAXLINE, stdin) != NULL) {
        while(1){

    std::cout<<"please enter the code: "<<std::endl;
    std::cin >> code;
    std::cout<<"enter the first time: "<<std::endl;
    std::cin >> FirstTime;
    std::cout<<"enter the second time: "<<std::endl;
    std::cin >> SecondTime;

    root["code"] = code;
    root["FirstTime"] = FirstTime;
    root["SecondTime"] = SecondTime;
 
        root.toStyledString();
        std::string send_buff = root.toStyledString();
	    char str2[2000];	
	    strcpy(str2,send_buff.c_str());
        std::cout<<"send success"<<std::endl;


        write(sockfd, str2, strlen(str2));

	printf("write success\n");
        n = read(sockfd, buf, MAXLINE);
	printf("read success\n");
        if (n == 0) {
            printf("the other side has been closed.\n");
            break;
        }
        else
            write(STDOUT_FILENO, buf, n);
    }



    close(sockfd);

    return 0;
}
