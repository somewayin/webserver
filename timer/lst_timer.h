#ifndef LST_TIMER
#define LST_TIMER

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/uio.h>

#include <time.h>
#include "../log/log.h"

//前向声明
class util_timer;

//用户资源
struct client_data
{
    sockaddr_in address; //客户端socket地址
    int sockfd; //通信socket的文件描述符
    util_timer *timer; //定时器
};

//定时器类
class util_timer
{
public:
    util_timer() : prev(NULL), next(NULL) {}

public:
    time_t expire; //超时时间
    
    void (* cb_func)(client_data *); //回调函数
    client_data *user_data; //用户数据
    util_timer *prev; //前向定时器
    util_timer *next; //后序定时器
};

//升序定时器链表容器
class sort_timer_lst
{
public:
    sort_timer_lst();
    ~sort_timer_lst();

    //插入新定时器
    void add_timer(util_timer *timer);
    //调整定时器在链表中的位置
    void adjust_timer(util_timer *timer);
    //删除定时器
    void del_timer(util_timer *timer);
    //工作函数，处理超时定时器
    void tick();

private:
    void add_timer(util_timer *timer, util_timer *lst_head);

    util_timer *head;
    util_timer *tail;
};

class Utils
{
public:
    Utils() {}
    ~Utils() {}

    void init(int timeslot);

    //对文件描述符设置非阻塞
    int setnonblocking(int fd);

    //将内核事件表注册读事件，ET模式，选择开启EPOLLONESHOT
    void addfd(int epollfd, int fd, bool one_shot, int TRIGMode);

    //信号处理函数
    static void sig_handler(int sig);

    //设置信号函数
    void addsig(int sig, void(handler)(int), bool restart = true);

    //定时处理任务，重新定时以不断触发SIGALRM信号
    void timer_handler();

    void show_error(int connfd, const char *info);

public:
    static int *u_pipefd;
    sort_timer_lst m_timer_lst;
    static int u_epollfd;
    //定时触发时间
    int m_TIMESLOT;
};

void cb_func(client_data *user_data);

#endif
