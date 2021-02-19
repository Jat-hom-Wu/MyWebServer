#include"threadpool.h"

template<typename T>
void ThreadPoll<T>::ThreadCreate(int thread_nums,int max_request){
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
            TheTask->run();
            std::cout<<"process test"<<std::endl;
            sleep(3);
        }
}

// int main(){
//     ThreadPoll<doing> poll1;
//     doing   t1(1);
//     doing t2(2);
//     doing t3(3);
//     poll1.ThreadCreate(3,10);
//     poll1.Addrequest(&t1);
//     poll1.Addrequest(&t2);
//     poll1.Addrequest(&t3);
//      poll1.Addrequest(&t1);
//     poll1.Addrequest(&t2);
//     poll1.Addrequest(&t3);
//    while(1);
// }