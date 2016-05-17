#ifndef _HTTPD_H_
#define _HTTPD_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/sendfile.h>
#include<sys/stat.h>
#include<errno.h>
#include<string.h>
#include<pthread.h>
#include<ctype.h>
#include<fcntl.h>

#define _BACK_LOG_ 5 //系统维护的监听数
#define _COMM_SIZE_ 1024//缓冲区的大小
#define MAIN_PAGE "index.html"//主页
#define HTTP_VERSION "HTTP/1.0"

#endif
