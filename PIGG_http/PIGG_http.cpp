#include "PIGG_http.h"

int PIGG_http_conn::PIGG_user_count = 0;
int PIGG_http_conn::PIGG_epollfd = -1;

/*********************************内部使用***********************************************/

//对文件描述符设置非阻塞
int set_non_blocking(int fd){
    int old_option = fcntl(fd,F_GETFL);
    int new_option = old_option | O_NONBLOCK ; 
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

// 将内核事件表注册读事件，ET模式，选择开启EPOLLONESHOT
void addfd(int epollfd,int fd, bool one_shot, int TrigMode){
    epoll_event event;
    event.data.fd = fd;

    if(TrigMode == 1)
        event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    else
        event.events = EPOLLIN | EPOLLRDHUP;
    
    if(one_shot)
        event.events |= EPOLLONESHOT;

    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    set_non_blocking(fd);
}

// 从内核时间表删除描述符
void remoefd(int epollfd, int fd){

}

// 将事件重置为EPOLLONESHOT
// 同一SOCKET只能被一个线程处理，不会跨越多个线程
// ************重要函数，需要理解***************/
void modfd(int epollfd, int fd, int ev ,int TrigMode){
    epoll_event event;
    event.data.fd = fd;

    if(TrigMode == 1)
        event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
    else   
        event.events = ev | EPOLLONESHOT | EPOLLRDHUP;

    epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&event);
}


/********************************************************************************/


void PIGG_http_conn::init_mysql_result(PIGG_connection_pool *connPool){

}


//关闭连接，关闭一个连接，客户总量减一
void PIGG_http_conn::close_conn(bool real_close){

}

//初始化连接,外部调用初始化套接字地址
void PIGG_http_conn::init(int sockfd,const sockaddr_in &addr, char *root, int TrigMode,
    int close_log, std::string user,std::string passwd,std::string sqlname){
    PIGG_sockfd = sockfd;
    PIGG_address = addr;

    addfd(PIGG_epollfd, sockfd, true, PIGG_TrigMode);
    PIGG_user_count++;  // 用来标记连接池的数量

    //当浏览器出现连接重置时，可能是网站根目录出错或http响应格式出错或者访问的文件中内容完全为空
    PIGG_doc_root = root;
    PIGG_TrigMode = TrigMode;
    PIGG_close_log = close_log;

    strcpy(sql_user, user.c_str());
    strcpy(sql_passwd, passwd.c_str());
    strcpy(sql_name, sqlname.c_str());

    init();
}

//初始化新接受的连接
//check_state默认为分析请求行状态
void PIGG_http_conn::init(){
    mysql = NULL;
    PIGG_check_status = CHECK_STATE_REQUEST_LINE;
    PIGG_linger = false;
    PIGG_method = GET;
    PIGG_url = 0;
    PIGG_version = 0;
    PIGG_host = 0;
    PIGG_start_line = 0;
    PIGG_checked_idex = 0;
    PIGG_read_idx = 0;
    PIGG_write_idx = 0;
    PIGG_cgi = 0;
    PIGG_content_length = 0;
    timer_flag = 0;
    improv = 0;

    bytes_to_send = 0;
    bytes_have_send = 0;
    PIGG_state = 0;

    memset(PIGG_read_buf, '\0',READ_BUFFER_SIZE);
    memset(PIGG_write_buf, '\0',WRITE_BUFFER_SIZE);
    memset(PIGG_real_file, '\0',FILE_NAME_LEN);
}


// 循环读取客户数据，直到无数据可读或对方关闭连接
// 非阻塞ET工作模式下，需要一次性将数据读完
// 在线程池中用到
// 在正常的程序中也需要用到
bool PIGG_http_conn::read_once(){
    if(PIGG_read_idx >= READ_BUFFER_SIZE){
        return false;
    }
    int bytes_read = 0;

    if(PIGG_TrigMode == 0){  //LT读取数据
        bytes_read = recv(PIGG_sockfd,PIGG_read_buf + PIGG_read_idx, READ_BUFFER_SIZE - PIGG_read_idx,0);
        PIGG_read_idx += bytes_read;
        if(bytes_read <= 0){
            return false;
        }
        return true;
    }else{ //ET读数据
        while(true){
            bytes_read = recv(PIGG_sockfd,PIGG_read_buf + PIGG_read_idx, READ_BUFFER_SIZE - PIGG_read_idx,0);
            if(bytes_read == -1){
                if(errno == EAGAIN || errno == EWOULDBLOCK)
                    break;
                return false;         
            }else if(bytes_read == 0){
                return false;
            }
            PIGG_read_idx += bytes_read;
        }
        return true;
    }
}


void PIGG_http_conn::unmap(){

}
bool PIGG_http_conn::write(){

}


// 整个的处理过程
// EPOLLIN/EPOLLOUT需要再去理解
void PIGG_http_conn::process(){
    PIGG_HTTP_CODE read_ret = process_read();   // 先把请求读取进来
    if(read_ret == NO_REQUEST){
        modfd(PIGG_epollfd, PIGG_sockfd, EPOLLIN, PIGG_TrigMode);
    }
    bool wriet_ret = process_write(read_ret);   // 再将要发送的东西写进去,根据得到的不同请求进行不同的发送
    if(!wriet_ret){
        // close_conn(true);
    }
    modfd(PIGG_epollfd, PIGG_sockfd, EPOLLOUT, PIGG_TrigMode);
}



