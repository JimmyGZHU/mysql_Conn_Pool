/*************************************************************************
    > File Name: sqlpool.cpp
    > Author: Jimmy
    > Mail: jimmy_gzhu2022@163.com 
    > Created Time: 2020年11月05日 星期四 20时09分20秒
 ************************************************************************/

#include "./sqlpool.h"

Pool::Pool(){
    this->m_freeconn = 0;
    this->m_curconn = 0;
}

Pool::~Pool(){
    Destroy();
}

int Pool::getFreeconn(){
    return m_freeconn;
}

bool Pool::init(string url, string dbName, string dbPswd, string dbUser, int port){
    m_url = url;
    m_dbName = dbName;
    m_port = port;
    m_dbPswd = dbPswd;
    m_dbUser = dbUser;
    for(int i = 0; i < maxconn; ++i){
        MYSQL* conn = NULL;
        conn = mysql_init(conn);
        if(conn == NULL){
            perror("mysql_init");
            exit(1);
        }
        conn = mysql_real_connect(conn, url.c_str(), dbUser.c_str(), dbPswd.c_str(), dbName.c_str(), port, NULL, 0);
        if(conn == NULL){
            perror("mysql_real_connect");
            exit(1);
        }          
        pool_.push_back(conn);
        ++m_freeconn;
    }
    rest = sem(m_freeconn);
    return true;
}

void Pool::Destroy(){
    Lock.lock();
    for(auto it = pool_.begin(); it != pool_.end(); ++it){
        MYSQL* item = *it;
        mysql_close(item);
    }
    m_curconn = 0;
    m_freeconn = 0;
    pool_.clear();  //清空list
    list<MYSQL*>().swap(pool_); //释放资源
    Lock.unlock();
}

MYSQL* Pool::GetConn(){
    if(pool_.size() == 0){
        return nullptr;
    }
    Lock.lock();

    rest.wait();  //信号量-1
    MYSQL* conn = pool_.front(); //取出一个连接
    pool_.pop_front();
    --m_freeconn;
    ++m_curconn;
    
    Lock.unlock();
    return conn;
}

void Pool::releaseConn(MYSQL* conn){
    if(conn == NULL){
        return;
    }
    Lock.lock();

    pool_.push_back(conn); //放进空闲链接队列
    ++m_freeconn;
    --m_curconn;
    rest.post();  //信号量+1
    
    Lock.unlock();
}

Pool* Pool::getInstance(){
    static Pool conn;  //全类共享
    return &conn;
}


RAIIPool::RAIIPool(MYSQL** conn ,Pool* pool_){
    *conn = pool_->GetConn();

    RAIIconn = *conn;
    RAIIpool = pool_;
}

RAIIPool::~RAIIPool(){
    RAIIpool->Destroy();
}
int main(){
    return 0;
}
