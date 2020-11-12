/*************************************************************************
    > File Name: locker.h
    > Author: Jimmy
    > Mail: jimmy_gzhu2022@163.com 
    > Created Time: 2020年11月05日 星期四 19时43分45秒
 ************************************************************************/

#include <iostream>
#include <pthread.h>
#include <semaphore.h>
using namespace std;


class locker
{
public:
    locker(){
        if(pthread_mutex_init(&mtx, nullptr) != 0){
            throw std::exception();
        }
    }
    ~locker(){
        pthread_mutex_destroy(&mtx);
    }
    bool lock(){
        return pthread_mutex_lock(&mtx) == 0;
    }
    bool unlock(){
        return pthread_mutex_unlock(&mtx) == 0;
    }
    pthread_mutex_t* GetMtx(){ //获取一把锁
        return &mtx;
    }
private:
    pthread_mutex_t mtx;
};

class sem
{
public:
    sem(int num){
        if(sem_init(&id, 0, num) != 0){  //线程共享
            throw std::exception();  
        }
    }
    sem(){
        if(sem_init(&id, 0, 0) != 0){
            throw std::exception();
        }
    }
    ~sem(){
        sem_destroy(&id);
    }
    bool wait(){
        return sem_wait(&id) == 0;
    }
    bool post(){
        return sem_post(&id) == 0;
    }
private:
    sem_t id;
};

class cond
{
public:
    cond(){
        if(pthread_cond_init(&mycond, nullptr) != 0){
            throw std::exception();
        }
    }
    ~cond(){
        pthread_cond_destroy(&mycond);
    }
    bool wait(pthread_mutex_t mtx){
        return pthread_cond_wait(&mycond, &mtx) == 0;
    }
    bool timeoutWait(pthread_mutex_t Mtx, struct timespec ts){
        pthread_mutex_lock(&mtx);
        int ret = pthread_cond_timedwait(&mycond, &Mtx, &ts);
        pthread_mutex_unlock(&mtx);
        return ret;
    }
    bool signal(pthread_cond_t con){
        return pthread_cond_signal(&con) == 0;
    }
    bool broadcast(){
        return pthread_cond_broadcast(&mycond) == 0;
    }
private:
    pthread_cond_t mycond;
    pthread_mutex_t mtx;
};
