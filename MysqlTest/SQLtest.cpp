#include <mysql/mysql.h>
#include<iostream>
#include"stdio.h"
#include<string.h>
const char *server = "localhost";
const char *user = "debian-sys-maint";
const char *password = "IRGYAJ4dyb32uIY2";         /* 此处改成你的密码 */
const char *database = "MylittleTest";    /* 数据库名字，比如mysql*/


int main(){

    MYSQL *con = mysql_init(NULL);
    MYSQL * fd;

     fd = mysql_real_connect(con, server, user, password, database, 3306, NULL, 0);
     std::cout<<"fd: "<<fd<<std::endl;
    int flag =  mysql_query(con, "INSERT INTO str VALUES('c++Add', 6);");
   // int flag = mysql_query(con, "select * from str");
    std::cout<<"flag: "<<flag<<std::endl;
    std::cout<<"ok"<<std::endl;
}