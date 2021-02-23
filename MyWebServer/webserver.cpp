#include"webserver.h"
#include <string.h>
#include"./conection/connet.h"
#include"string.h"

 int idx = 0;


WebServer::WebServer(){
//创建工作线程需要执行的对象
users = new json_conn[MAX_FD];

}

WebServer::~WebServer(){
//delete object
delete[] users;
delete m_pool;

}

void WebServer::init(int port , std::string user, std::string passWord,std::string databaseName,
              int log_write , int opt_linger, int trigmode, int sql_num,
              int thread_num, int close_log, int actor_model)
              {
                  //初始化赋值
                    m_port = port;
                    m_user = user;
                    m_passWord = passWord;
                    m_databaseName = databaseName;
                    m_sql_num = sql_num;
                    m_thread_num = thread_num;
                    m_log_write = log_write;
                    m_OPT_LINGER = opt_linger;
                    m_TRIGMode = trigmode;
                    m_close_log = close_log;
                    m_actormodel = actor_model;
              }

void WebServer::sql_pool()
{
    m_connPool = connection_pool::GetInstance();        //单例模式获得一个对象
    m_connPool->init("localhost", m_user, m_passWord, m_databaseName, 3306, m_sql_num, m_close_log);

    //初始化数据库读取表
    //users->initmysql_result(m_connPool);
}

void WebServer::thread_pool()
{
    m_pool = new ThreadPoll<json_conn>;
    m_pool->ThreadCreate(8,100,m_connPool);
    
    m_pool->m_connPool = m_connPool;

}

void WebServer::eventListen()
{
    m_listenfd = socket(PF_INET, SOCK_STREAM, 0);

    struct linger tmp = {1, 1};
     setsockopt(m_listenfd, SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    //address.sin_port = htons(m_port);
    address.sin_port = htons(8080);

    int flag = 1;
    setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    ret = bind(m_listenfd, (struct sockaddr *)&address, sizeof(address));
    assert(ret >= 0);
    ret = listen(m_listenfd, 5);
    assert(ret >= 0);

     epoll_event events[MAX_EVENT_NUMBER];
    m_epollfd = epoll_create(5);
    assert(m_epollfd != -1);

     epoll_event event;
     event.data.fd = m_listenfd;

    event.events = EPOLLIN | EPOLLRDHUP;

    epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_listenfd, &event);
    setnonblocking(m_listenfd);
    
    json_conn::m_epollfd = m_epollfd;

}

void WebServer::dealwithread(int sockfd)
 {
     if (users[sockfd].read_once())
        {
           // LOG_INFO("deal with the client(%s)", inet_ntoa(users[sockfd].get_address()->sin_addr));

            //若监测到读事件，将该事件放入请求队列
            m_pool->Addrequest(users + sockfd);             //什么语法？？ //数组首地址+sockfd确定缓冲区地址
        }
        else
        {
            std::cout<<"none data now "<<std::endl;
        }
 }

void WebServer::dealwithwrite(int sockfd)
{
    
    if (users[sockfd].mywrite())
        {
           // LOG_INFO("send data to the client(%s)", inet_ntoa(users[sockfd].get_address()->sin_addr));

           
        }

}

bool WebServer::dealclinetdata()
{
     int num = 0;
    char str[INET_ADDRSTRLEN];

    struct sockaddr_in client_address;
    socklen_t client_addrlength = sizeof(client_address);

    int connfd = accept(m_listenfd, (struct sockaddr *)&client_address, &client_addrlength);
        if (connfd < 0)
        {
           std::cout<<"accept error "<<std::endl;
            return false;
        }

        //test2.1
        printf("received from %s at PORT %d\n", 
                        inet_ntop(AF_INET, &client_address.sin_addr, str, sizeof(str)), 
                        ntohs(client_address.sin_port));
                printf("cfd %d---client %d\n", connfd, ++num);

    epoll_event event;
    event.data.fd = connfd;
    event.events = EPOLLIN | EPOLLRDHUP;
    event.events |= EPOLLONESHOT;
    epoll_ctl(m_epollfd, EPOLL_CTL_ADD, connfd, &event);
    //setnonblocking(connfd);                           //设置非阻塞

        // timer(connfd, client_address);
    users[connfd].init(connfd, client_address, m_root, m_CONNTrigmode, m_close_log, m_user, m_passWord, m_databaseName);
    return true;
}

    char buf[30];
    int n = 0;

void WebServer::eventLoop()
{
    bool stop_server = false;



    while (!stop_server)
    {
        int number = epoll_wait(m_epollfd, events, MAX_EVENT_NUMBER, -1);
        if (number < 0 && errno != EINTR)
        {
            std::cout<<"epoll error"<<std::endl;
            break;
        }

        for (int i = 0; i < number; i++)
        {
            int sockfd = events[i].data.fd;

            //处理新到的客户连接
            if (sockfd == m_listenfd)
            {
                bool flag = dealclinetdata();
                if (false == flag)
                    continue;
            }
            else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
            {
                //服务器端关闭连接，移除对应的定时器
                std::cout<<"timer error"<<", The Client closed"<<std::endl;

                //增加移除节点
        
            }
            //处理信号
            else if ((sockfd == m_pipefd[0]) && (events[i].events & EPOLLIN))
            {

                std::cout<<"handler the signal"<<std::endl;
            }
            //处理客户连接上接收到的数据
            else if (events[i].events & EPOLLIN)
            {
                dealwithread(sockfd);
            //     int bytes_read = 0;
            //    // n = read(sockfd,buf,30);
            //    bytes_read = recv(sockfd, buf , 1000 , 0);
            //     idx += bytes_read;
            //      for (i = 0; i < bytes_read; i++)
            //             buf[i] = toupper(buf[i]);   //转大写,写回给客户端
            //     write(STDOUT_FILENO,buf, bytes_read );

            //     epoll_event event;
            //      event.data.fd = sockfd;
            //      event.events = EPOLLIN | EPOLLRDHUP;
            // event.events |= EPOLLONESHOT;
            // epoll_ctl(m_epollfd, EPOLL_CTL_MOD, sockfd, &event);
            //  setnonblocking(sockfd);
            }
            else if (events[i].events & EPOLLOUT)
            {
               
                dealwithwrite(sockfd);

                 epoll_event event;
                 event.data.fd = sockfd;
                 event.events = EPOLLIN | EPOLLRDHUP;
                event.events |= EPOLLONESHOT;
                epoll_ctl(m_epollfd, EPOLL_CTL_MOD, sockfd, &event);
                setnonblocking(sockfd);
                
            }
        }

    }



}
















