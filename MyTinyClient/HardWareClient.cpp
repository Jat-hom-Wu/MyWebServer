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

 int Gettime()
{
    using namespace std;
    using std::chrono::system_clock;

    auto now = system_clock::now();
    auto now_c = system_clock::to_time_t(now);
    ostringstream ss;
    ss << put_time(localtime(&now_c), "%m%d%H%M");
    string timeid = ss.str();
    
     int a;
    stringstream sa;
    sa << timeid;
    sa >> a;
    return a;
}   
 
int GetCode()
{
        srand((unsigned)time(NULL));
        int randNumber1 = rand();
        int randNumber2 = rand()%10;//取0~9范围内的随机数
        return randNumber1;
}

int Getdata(){
        int r = 0;
        int n = 30;
        int m = 5;
        srand( (unsigned)time( NULL ) );
        r = rand()%(n-m+1)+m;
        return r;
}


int main(int argc, char *argv[])
{
    struct sockaddr_in servaddr;
    char buf[MAXLINE];
    int sockfd, n;
    int i=20;

    int ThisCode = GetCode();
    std::cout<<"The HardWareCode: "<<ThisCode<<std::endl;


    Json::Value root;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
   // inet_pton(AF_INET, "121.5.140.194", &servaddr.sin_addr);
   inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
    servaddr.sin_port = htons(SERV_PORT);



    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    while (1) {
        
        root["user"] = 0;                        //0为硬件客户端，1为pc客户端
        root["code"] = ThisCode ;
        root["time"] = Gettime();                            
        root["temper"] =Getdata();
        root["co2"] =Getdata();    
        root["co"] =Getdata();
        root["no2"] =Getdata();
        root["no"] =     Getdata();

        root.toStyledString();
        std::string send_buff = root.toStyledString();
	    char str2[2000];	
	    strcpy(str2,send_buff.c_str());
        std::cout<<"send success"<<std::endl;

        write(sockfd, str2, strlen(str2));

        sleep(30);

    }
    close(sockfd);

    return 0;
}

