#include<iostream>
#include"webserver.h"








int main()
{
    WebServer smart;
    smart.thread_pool();
    smart.eventListen();
    smart.eventLoop();

}