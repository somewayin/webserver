#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H
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
#include <map>

#include "../lock/locker.h"
#include "../CGImysql/sql_connection_pool.h"
#include "../timer/lst_timer.h"
#include "../log/log.h"

class http_conn
{
public:
    static const int FILENAME_LEN = 200;
    static const int READ_BUFFER_SIZE = 2048;
    static const int WRITE_BUFFER_SIZE = 1024;
    enum METHOD
    {
        GET = 0,
        POST,
        HEAD,
        PUT,
        DELETE,
        TRACE,
        OPTIONS,
        CONNECT,
        PATH
    };
    enum CHECK_STATE
    {
        CHECK_STATE_REQUESTLINE = 0,
        CHECK_STATE_HEADER,
        CHECK_STATE_CONTENT
    };
    enum HTTP_CODE
    {
        NO_REQUEST,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURCE,
        FORBIDDEN_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION
    };

    //解析每行的返回状态
    enum LINE_STATUS
    {
        LINE_OK = 0,
        LINE_BAD,
        LINE_OPEN
    };

public:
    http_conn() {}
    ~http_conn() {}

public:
    void init(int sockfd, const sockaddr_in &addr, char *, int, int, string user, string passwd, string sqlname);
    void close_conn(bool real_close = true);
    void process();
    bool read_once();
    bool write();
    sockaddr_in *get_address()
    {
        return &m_address;
    }
    void initmysql_result(connection_pool *connPool);
    int timer_flag;
    int improv;


private:
    void init();
    HTTP_CODE process_read();
    bool process_write(HTTP_CODE ret);
    HTTP_CODE parse_request_line(char *text);
    HTTP_CODE parse_headers(char *text);
    HTTP_CODE parse_content(char *text);
    HTTP_CODE do_request();
    char *get_line() { return m_read_buf + m_start_line; };
    LINE_STATUS parse_line();
    void unmap();
    bool add_response(const char *format, ...);
    bool add_content(const char *content);
    bool add_status_line(int status, const char *title);
    bool add_headers(int content_length);
    bool add_content_type();
    bool add_content_length(int content_length);
    bool add_linger();
    bool add_blank_line();

public:
    static int m_epollfd;     //每个http连接共享epollfd
    static int m_user_count;  //每个http连接共享连接数
    MYSQL *mysql; //数据库连接
    int m_state;  //读为0, 写为1

private:
    int m_sockfd; //与客户通信的sockfd
    sockaddr_in m_address; //客户端socket地址
    char m_read_buf[READ_BUFFER_SIZE]; //读缓冲区
    long m_read_idx; //标识实时的读取字节数
    long m_checked_idx; //标识实时的解析字节数
    int m_start_line;
    char m_write_buf[WRITE_BUFFER_SIZE]; //写缓冲区
    int m_write_idx;
    CHECK_STATE m_check_state;
    METHOD m_method;
    char m_real_file[FILENAME_LEN];
    char *m_url;
    char *m_version;
    char *m_host;
    long m_content_length;
    bool m_linger; //长连接
    char *m_file_address; //内存映射文件的地址
    struct stat m_file_stat;
    struct iovec m_iv[2];
    int m_iv_count;
    int cgi;        //是否使用CGI校验
    char *m_string; //存储请求体数据
    int bytes_to_send;
    int bytes_have_send;
    char *doc_root;

    map<string, string> m_users;
    int m_TRIGMode; //触发模式，ET or LT
    int m_close_log;

    char sql_user[100]; //连接对应的用户名
    char sql_passwd[100]; //连接对应的密码
    char sql_name[100]; //连接对应的数据库名
};

#endif
