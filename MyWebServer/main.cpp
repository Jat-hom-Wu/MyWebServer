#include<iostream>
#include"webserver.h"




int main()
{
    string user = "debian-sys-maint";
    string passwd = "IRGYAJ4dyb32uIY2";
    string databasename = "MylittleTest";

    WebServer smart;
    smart.init(
        8080,
        user,
        passwd,
        databasename,
        1,1,1,1,1,1,1
    );
    smart.sql_pool();
    smart.thread_pool();
    smart.eventListen();
    smart.eventLoop();

}