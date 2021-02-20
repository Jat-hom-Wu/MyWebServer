#ifndef __THREADPOOL_H_
#define __THREADPOOL_H_

#include<iostream>
#include<string>
#include <pthread.h>
#include<list>
#include"../locker/locker.h"
#include"stdio.h"
#include"unistd.h"
#include"../sql_conn/sqlconn.h"



template <typename T>
class ThreadPoll{
    public:
    int m_thread_nums;
    int m_max_request;
    std::list<T*> m_taskqueue;
    pthread_t* m_thread_id;
    locker m_queuelocker;       //保护请求队列的互斥锁
    sem m_queuestat;            //是否有任务需要处理


    void ThreadCreate(int thread_nums, int max_request, connection_pool *connPool);
    bool Addrequest(T * request);
    static  void* work(void *arg);
    ~ThreadPoll();

        connection_pool *m_connPool ; 
};

class doing{                                //task object
    public:
    int m_i;
    int m_socket;
    char* m_buf;
    int m_n;

    void doing_set(int socket, char* buf,int n){
        m_socket = socket;
        m_buf= buf;
        m_n = n;
    }
    void run(){
                  for (int i = 0; i < m_n; i++)
                    m_buf[i] = toupper(m_buf[i]);   //转大写,写回给客户端
                     write(STDOUT_FILENO,m_buf, m_n);
                    write(m_socket, m_buf, m_n);
                    std::cout<<"no problem"<<std::endl;
                    
    }

};


template<typename T>
void ThreadPoll<T>::ThreadCreate(int thread_nums,int max_request,  connection_pool *connPool) 
{
    m_connPool = connPool;

    //error fixing
    m_thread_nums = thread_nums;
    m_max_request = max_request;
    m_thread_id = new pthread_t[thread_nums];
    if(!m_thread_id) throw std::exception();
    for(int i =0; i<m_thread_nums; i++){
        if(pthread_create(m_thread_id+i, NULL, work,this ) != 0 ){
            delete[] m_thread_id;
            throw std::exception();
        }
        if(pthread_detach(m_thread_id[i])){
            delete[] m_thread_id;
        }
    }
}

template <typename T>
bool ThreadPoll<T>::Addrequest(T *request){
        m_queuelocker.lock();
        if(m_taskqueue.size() >= m_max_request){
            m_queuelocker.unlock();
            return false;
        }
        m_taskqueue.push_back(request);
        m_queuelocker.unlock();
        m_queuestat.post();
        return true;
}

template<typename T>
ThreadPoll<T>::~ThreadPoll()
{
        delete[] m_thread_id;
}



template<typename T>
void* ThreadPoll<T>::work(void *arg){
        ThreadPoll * poll = (ThreadPoll*) arg;
        while(1){
            poll->m_queuestat.wait();
            poll->m_queuelocker.lock();
            if(poll->m_taskqueue.empty()){
                poll->m_queuelocker.unlock();
                continue;
            }
            T* TheTask = poll->m_taskqueue.front();
            poll->m_taskqueue.pop_front();
            poll->m_queuelocker.unlock();
            if(!TheTask)     continue;
            connectionRAII mysqlcon(&TheTask->mysql, poll->m_connPool);             //???
            TheTask->process();
            
        }
}



#endif
