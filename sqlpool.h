/*************************************************************************
    > File Name: sqlpool.h
    > Author: Jimmy
    > Mail: jimmy_gzhu2022@163.com 
    > Created Time: 2020年11月05日 星期四 19时28分57秒
 ************************************************************************/

#pragma once

#include "./locker.h"
#include <iostream>
#include <string>
#include <list>
#include <pthread.h>
#include <mysql/mysql.h>
using namespace std;

class Pool{
public:
    ~Pool();
    int getFreeconn();
    bool init(string url, string dbName, string dbPswd, string User, int port);
    void Destroy();  //销毁连接池
    void releaseConn(MYSQL* conn);   //释放一个连接
    MYSQL* GetConn();     //获取一个空闲连接
    Pool* getInstance();  //单例连接
    
private:
    Pool();
    list<MYSQL*> pool_;
    string m_dbName;
    string m_dbPswd;
    string m_dbUser;
    string m_port;
    string m_url;

    constexpr static int maxconn = 10; //最大连接
    int m_curconn;
    int m_freeconn;    //空闲连接
    
    locker Lock;     //原子操作
    sem rest;   //信号量类检测剩余空闲连接数
};


//真正实现RAII - 创建对象即获得一个连接, 析构函数则释放资源
class RAIIPool{
public:
    RAIIPool(MYSQL** conn, Pool* pool_);
    ~RAIIPool();
private:
    MYSQL* RAIIconn;
    Pool* RAIIpool;
};
