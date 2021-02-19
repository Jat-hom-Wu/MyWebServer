#include"connet.h"
#include<string>
#include<iostream>

//对文件描述符设置非阻塞
int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

void addfd(int epollfd, int fd, bool one_shot, int TRIGMode)
{
    epoll_event event;
    event.data.fd = fd;

    if (1 == TRIGMode)
        event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    else
        event.events = EPOLLIN | EPOLLRDHUP;

    if (one_shot)
        event.events |= EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

void modfd(int epollfd, int fd, int ev, int TRIGMode)
{
    epoll_event event;
    event.data.fd = fd;

    if (1 == TRIGMode)
        event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
    else
        event.events = ev | EPOLLONESHOT | EPOLLRDHUP;

    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

//从内核时间表删除描述符
void removefd(int epollfd, int fd)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}


void json_conn::init(int sockfd, const sockaddr_in &addr, char *root, int TRIGMode,
                     int close_log, std::string user, std::string passwd, std::string sqlname)
{
    m_sockfd = sockfd;
    m_address = addr;

    addfd(m_epollfd, sockfd, true, m_TRIGMode);
    m_user_count++;

    //当浏览器出现连接重置时，可能是网站根目录出错或http响应格式出错或者访问的文件中内容完全为空
    doc_root = root;
    m_TRIGMode = TRIGMode;
    m_close_log = close_log;

    strcpy(sql_user, user.c_str());
    strcpy(sql_passwd, passwd.c_str());
    strcpy(sql_name, sqlname.c_str());

    init();
}
    
void json_conn::init()
{
    mysql = NULL;
    bytes_to_send = 0;
    bytes_have_send = 0;
    m_linger = false;
    m_url = 0;
    m_version = 0;
    m_content_length = 0;
    m_host = 0;
    m_start_line = 0;
    m_checked_idx = 0;
    m_read_idx = 0;
    m_write_idx = 0;
    cgi = 0;
    m_state = 0;
    timer_flag = 0;
    improv = 0;

    memset(m_read_buf, '\0', READ_BUFFER_SIZE);
    memset(m_write_buf, '\0', WRITE_BUFFER_SIZE);
    memset(m_real_file, '\0', FILENAME_LEN);
}

int json_conn::m_user_count = 0;
int json_conn::m_epollfd = -1;
    
void json_conn::close_conn(bool real_close )
{
        if (real_close && (m_sockfd != -1))
    {
        printf("close %d\n", m_sockfd);
        removefd(m_epollfd, m_sockfd);
        m_sockfd = -1;
        m_user_count--;
    }
}
    
bool json_conn::read_once()
{
    if (m_read_idx >= READ_BUFFER_SIZE)
    {
        return false;
    }
    int bytes_read = 0;

    //LT读取数据
        bytes_read = recv(m_sockfd, m_read_buf , READ_BUFFER_SIZE , 0);
        m_read_idx = bytes_read;

        // if (bytes_read <= 0)
         if (bytes_read < 0)
        {
            std::cout<<"LT读取数据失败"<<std::endl;
            return false;
        }
        return true;
}
    
    
bool json_conn::mywrite()
{
    //int temp = 0;

    // if (bytes_to_send == 0)
    // {
    //     modfd(m_epollfd, m_sockfd, EPOLLIN, m_TRIGMode);
    //     init();
    //     return true;
    // }
                   
                //std::cout<<"write test"<<std::endl;
                     write(STDOUT_FILENO,m_read_buf, m_read_idx);
                    //write(m_sockfd, m_read_buf+m_read_idx, m_read_idx);
                    
                    return true;

    // while (1)
    // {
    //     temp = writev(m_sockfd, m_iv, m_iv_count);

    //     if (temp < 0)
    //     {
    //         if (errno == EAGAIN)
    //         {
    //             modfd(m_epollfd, m_sockfd, EPOLLOUT, m_TRIGMode);
    //             return true;
    //         }
    //         //unmap();
    //         return false;
    //     }

    //     bytes_have_send += temp;                        //发送的数据大小
    //     bytes_to_send -= temp;                          //还需要发送的数据大小
    //     if (bytes_have_send >= m_iv[0].iov_len)         //发送完
    //     {
    //         m_iv[0].iov_len = 0;
    //     }
    //     else                                            //未发送完
    //     {
    //         m_iv[0].iov_base = m_write_buf + bytes_have_send;
    //         m_iv[0].iov_len = m_iv[0].iov_len - bytes_have_send;
    //     }

    //     if (bytes_to_send <= 0)                     
    //     {
    //         //unmap();
           //  modfd(m_epollfd, m_sockfd, EPOLLIN, 0);

    //         init();
    //         return true;
    //     }
    // }
}

int json_conn::process_read()               //identify function
{
    return 2;
}

bool json_conn::process_write(int ret)              //identify function
{
    int temp = ret;
    temp =0;
    
   for (int i = 0; i < m_read_idx; i++)
    m_read_buf[i] = toupper(m_read_buf[i]); 

    
    // m_iv[0].iov_base = m_read_buf+m_read_idx;
    // m_iv[0].iov_len = m_read_idx;
    // m_iv_count = 1;
    // bytes_to_send = m_read_idx;
    return true;
}

void json_conn::process()                       //needing to write again!!!!
{
     int read_ret = process_read();
    if (read_ret == 1)
    {
        modfd(m_epollfd, m_sockfd, EPOLLIN, 0);
        return;
    }
    bool write_ret = process_write(read_ret);
    if (!write_ret)
    {
        close_conn();
    }
    modfd(m_epollfd, m_sockfd, EPOLLOUT, 0);
}