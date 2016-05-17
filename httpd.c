#include"httpd.h"

void usage(const char *proc)
{
	printf("./%s [IP] [PORT]\n", proc);
}

void print_log(const char *fun, int line, int err_no, const char *err_str)
{
	printf("[%s: %d] [%d] [%s]\n", fun, line, err_no, err_str);
}

//以行为单位读取客户端的数据
//返回值表示读取的字符数，0表示读取失败
int get_line(const int sock, char *buf, int max_len)
{
	if( !buf || max_len < 0){
		return 0;
	}
	int i = 0;//累计读取的字符数
	int n = 0;
	char c = '\0';
	while( i < max_len-1 && c != '\n'){
		n = recv(sock, &c, 1, 0);//以阻塞方式每次读取一个字符到C
		if( n > 0){
			if( c == '\r' ){//mac os下回车为\r，win os下回车为\r\n，将回车统一为\n
				n = recv(sock, &c, 1, MSG_PEEK);//MSG_PEEK表示以嗅探的方式下一字符
				if( n > 0 && c == '\n'){//windows
					recv(sock, &c, 1, 0);//正式从缓冲区中读出下一字符
				}else{
					c = '\n';
				}
			}
			buf[i++] = c;
		}else{//recv失败
			c = '\n';
		}
	}
	buf[i] = '\0';
	return i;
}

static void bad_request(int client)
{
	char buf[1024] = {'\0'};
	sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-type: text/html\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<h>400</h><p>Bad request！</p>");
	send(client, buf, strlen(buf), 0);
}

static void not_found(int client)
{
	char buf[1024] = {'\0'};
	sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-type: text/html\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<h>404</h><p>Not find you request's file！</p>");
	send(client, buf, strlen(buf), 0);
}

static void server_error(int client)
{
	char buf[1024] = {'\0'};
	sprintf(buf, "HTTP/1.0 500 SERVER ERROR\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-type: text/html\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<h>500</h><p>Server inner error！</p>");
	send(client, buf, strlen(buf), 0);

}

void echo_error_to_client(int client, int error_code)
{
	switch(error_code){
		case 400://request error
			bad_request(client);
			break;
		case 404://not found
			not_found(client);
			break;
		case 500://server error
			server_error(client);
			break;
		case 503://server unavailable
			//server_unavilable(client);
			break;
		default:
			break;
	}
	close(client);
}
void print_debug(const char *msg)
{
#ifdef _DEBUG_
	printf("print_debug:%s\n", msg);
#endif
}

void echo_html(int client, const char *path, unsigned int file_size)
{
	//使用send_file以零拷贝的方式完文件的拷贝
	if( !path ){
		return;
	}
	int in_fd = open(path, O_RDONLY);//以只读方式打开文件
	if(in_fd < 0){
		print_debug("open html error");
		echo_error_to_client(client, 404);//打开文件失败
		return;
	}
	//向客户端发送响应行
	char echo_line[_COMM_SIZE_];
	memset(echo_line, 0, sizeof(echo_line));
	strncpy(echo_line, HTTP_VERSION, strlen(HTTP_VERSION));//协议版本
	strcat(echo_line, " 200");//状态码
	strcat(echo_line, " OK");//状态码后述
	strcat(echo_line, "\r\n\r\n");
	send(client, echo_line, strlen(echo_line), 0);//发送响应行
	print_debug("send echo head success");

	if( sendfile(client, in_fd, 0, file_size) < 0 ){//sendfile(out,in,offset,size);
		close(in_fd);
		echo_error_to_client(client, 500);//sendfile调用失败
		return;
	}
	close(in_fd);
}

void exe_cgi(int sock_client, const char* path, const char* method, const char* query_string)
{
	char buf[_COMM_SIZE_];
	int numchars = 0;//读取到的字符个数
	int content_length = 0;//content_length 长度

	if(strcasecmp(method, "GET") == 0){//GET方式
		clear_header(sock_client);
	}else{//POST方式
		do{
			memset(buf, 0, sizeof(buf));
			numchars = get_line(sock_client, buf, sizeof(buf));//先读取ConLen字段得到消息正文的长度
			if(strncasecmp(buf, "Content-Length:", strlen("Content-Length")) == 0){
				content_length = atoi(&buf[16]);//16为真正length的开始位置
			}
		}while(numchars>0 && strcmp(buf, "\n")!=0);

		if( content_length == -1 ){
			echo_error_to_client(sock_client, 500);
			return;
		}
	}

	//向客户端发送响应信息
	memset(buf, 0, sizeof(buf));
	strcpy(buf, HTTP_VERSION);
	strcat(buf, " 200 OK\r\n\r\n");
	send(sock_client, buf, strlen(buf), 0);

	//用来双向通信的管道
	int cgi_input[2] = {0};
	int cgi_output[2] = {0};

	if( pipe(cgi_input) == -1 ){
		echo_error_to_client(sock_client, 500);
		return;
	}
	if( pipe(cgi_output) == -1){
		echo_error_to_client(sock_client, 500);
		close(cgi_input[0]);
		close(cgi_input[1]);
		return;
	}

	//创建子进程用来处理程序
	pid_t id;
	if( (id = fork()) < 0){//创建子进程失败
		close(cgi_input[0]);
		close(cgi_input[1]);
		close(cgi_output[0]);
		close(cgi_output[1]);
		echo_error_to_client(sock_client, 500);
		return;
	}else if( id ==0 ){//子进程处理逻辑
		char method_env[_COMM_SIZE_];//通过将参数写入环境变量的方式来实现程序替换后仍可使用替换前函数的参数
		char query_env[_COMM_SIZE_];
		char content_len_env[_COMM_SIZE_];//表示POST方式请求时正文的长度
		memset(method_env, 0, sizeof(method_env));
		memset(query_env, 0, sizeof(query_env));
		memset(content_len_env, 0, sizeof(content_len_env));
		
		close(cgi_input[1]);//子进程的数据从父进程中获得，故关闭子进程输入管道的写端
		close(cgi_output[0]);
		print_debug("child pipe close done");
		//将子进程输入重定向为cgi_input[0]
		dup2(cgi_input[0], 0);
		//将子进程输出重定向为cgi_output[1]
		dup2(cgi_output[1], 1);

		sprintf(method_env, "REQUEST_METHOD=%s", method);
		putenv(method_env);//将method参数写入到环境变量中
		
		if( strcasecmp("GET", method) == 0){//若为GET方式仍需传递参数query_string
			sprintf(query_env, "QUERY_STRING=%s", query_string);
			putenv(query_env);
		}else{//POST方式，需得到请求正文的长度
			sprintf(content_len_env, "CONTENT_LENGTH=%d", content_length);
			putenv(content_len_env);
		}

		//替换并执行目标程序
		execl(path, path, NULL);
		exit(1);
	}else{//父进程处理逻辑
		close(cgi_input[0]);
		close(cgi_output[1]);

		int i = 0;
		char c = '\0';//若为POST方式则需从socket中读取数据
		if( strcasecmp("POST", method) == 0){
			for(; i < content_length; ++i){
				recv(sock_client, &c, 1, 0);
				write(cgi_input[1], &c, sizeof(c));
			}
		}
		//读取子进程写入的数据
		while( read(cgi_output[0], &c, 1) > 0){
			send(sock_client, &c, 1, 0);
		}
		close(cgi_input[1]);
		close(cgi_output[0]);

		waitpid(id, NULL, 0);//防止僵尸进程
	}
}

//清理输出缓冲区
void clear_header(int client)
{
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	int ret = 0;
	do{
		ret = get_line(client, buf, sizeof(buf));
	}while( ret >0  && strcmp(buf, "\n") != 0 );
}

void *accept_request(void *arg)
{
	pthread_detach(pthread_self());//将线程设置为分离线程

	int sock_client = (int)arg;

	char buffer[_COMM_SIZE_];
	char method[_COMM_SIZE_/10];//请求方式
	char url[_COMM_SIZE_];//请求的连接
	memset(buffer, '\0', sizeof(buffer));
	memset(method, '\0', sizeof(method));
	memset(url, '\0', sizeof(url));

/*
#ifdef _DEBUG_//用来打印请求的Http报头仅供调试时使用
	//查看客服端发送的数据
	while( get_line(sock_client, buffer, sizeof(buffer))){
		printf("%s", buffer);
		fflush(stdout);
	}
	printf("\n");
	printf("debug:sock_client:%d\n", sock_client);
#endif
*/
	/*-----------------------------处理请求行---------------------------------*/
	//获得客户端的请求头
	if( get_line(sock_client, buffer, sizeof(buffer)) < 0){
		echo_error_to_client(sock_client, 500);//错误信息回显给客户端
		return (void *)-1;
	}

	int i = 0;//用来表示method的下标
	int j = 0;//buffer的下标
	//获得请求的方式
	while( !isspace(buffer[j]) && i < sizeof(method)-1 && j<sizeof(buffer)){//不是空格
		method[i++] = buffer[j++];//get /index.html http/1.1
	}
	//过滤掉m)ethod与URL之间多余的空格，循环结束后j指向URL的有效首字母
	while( isspace(buffer[j]) && j<sizeof(buffer)){
		j++;
	}
	//提取出URL
	i = 0;
	while( !isspace(buffer[j]) && i<sizeof(url)-1 && j<sizeof(buffer)){
		url[i++] = buffer[j++];
	}
	printf("test: url->%s | method->%s\n", url, method);
	//服务器仅可以处理GET与POST请求方式，其他请求方式无效
	if( strcasecmp(method, "GET") && strcasecmp(method, "POST")){
		echo_error_to_client(sock_client, 400);
		return -1;
	}
	int cgi = 0;//是否使用CGI
	//POST 请求方式
	if( strcasecmp(method, "POST") == 0){
		cgi = 1;
	}
	char *query_string = url;//判断URL是否传递参数的辅助指针
	//GET请求方式：请求资源，传递参数
	if( strcasecmp(method, "GET") == 0){
		while( *query_string != '?' && *query_string != '\0'){//查找URL是否包含？
			query_string++;
		}
		if( *query_string == '?' ){//url中传递参数 url=/**?xx=xx
			*query_string = '\0';//将URL分割为两部分，url指向正常URL,query_string指向参数
			query_string++;
			cgi = 1;//URL中含有?表示一定有参数，则设置cgi为1
		}
	}
	/*---------------------------结束处理请求行------------------------------------*/

	/*---------------------------分析URL的资源信息----------------------------------*/
	char path[_COMM_SIZE_];//表示请求资源的路径
	memset(path, '\0', sizeof(path));
	sprintf(path, "htdocs%s", url);//URL上拼接htdocs
	if( path[strlen(path)-1] == '\\'){
		strcat(path, MAIN_PAGE);//若URL仅为/ 则表示请求主页
	}

	struct stat st;
	if( stat(path, &st) < 0){//调用函数失败，文件不存在
		clear_header(sock_client);
		echo_error_to_client(sock_client, 404);//错误消息回馈给客户端
		return ;
	}else{//请求的文件存在
		print_debug("this file exist");
		if( S_ISDIR(st.st_mode)){//根据成员st_mode判断文件的属性
			strcat(path, "/");
			strcat(path, MAIN_PAGE);
		}else if(st.st_mode & S_IXUSR || st.st_mode & S_IXGRP ||
				 st.st_mode & S_IXOTH){
			cgi = 1;//具备可执行权限,CGI设为1
		}else{

		}
	}
	/*---------------------------结束分析URL资源信息--------------------------------*/

	/*------------------------------是否为请求CGI------------------------------------*/
	if(cgi){
		print_debug("if cgi judge success");
		exe_cgi(sock_client, path, method, query_string);//执行CIG程序
	}else{
		clear_header(sock_client);//清理头部信息
		print_debug("start echo html");
		echo_html(sock_client, path, st.st_size);//返回请求的网页
	}
	/*------------------------------处理CGI失败--------------------------------------*/

	close(sock_client);//关闭连接
	return NULL;
}

//成功返回合理的socket值，负责终止程序
int start(const short port, const char *ip)
{
	int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if( listen_sock == -1 ){
		print_log(__FUNCTION__, __LINE__, errno, strerror(errno));//创建失败时，打印日志信息
		//strerror函数返回类型为char* 负责打印错误码对应的错误信息
		exit(1);
	}

	int flag = 1;
	setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

	struct sockaddr_in local;//设置本机的网络信息
	local.sin_family = AF_INET;
	local.sin_port = htons(port);//小端字节序转为大端字节序
	//local.sin_addr.s_addr = htonl(INADDR_ANY);//自己选择一个本机可用IP地址进行绑定
	inet_pton(AF_INET, ip, &local.sin_addr);

	socklen_t len = sizeof(local);//将监听socket绑定到指定端口
	if( bind(listen_sock, (struct sockaddr*)&local, len) == -1 ){
		print_log(__FUNCTION__, __LINE__, errno,  strerror(errno));//绑定失败时，打印日志信息
		exit(2);
	}

	if( listen(listen_sock, _BACK_LOG_) == -1){
		print_log(__FUNCTION__, __LINE__, errno, strerror(errno));//监听失败时，打印日志信息
		exit(3);
	}
	return listen_sock;
}

int main(int argc, char *argv[])
{
	if( argc != 3){
		usage(argv[0]);//程序启动方式
		exit(1);
	}
	int port = atoi(argv[2]);//得到端口号
	const char *ip = argv[1];
	int sock = start(port, ip);//服务器的监听套接字

	struct sockaddr_in client;
	socklen_t len = 0;
	while(1){
		int new_sock = accept(sock, (struct sockaddr*)&client, &len);
		if( new_sock < 0){
			print_log(__FUNCTION__, __LINE__, errno, strerror(errno));//接收失败，结束本次循环
			continue;
		}
		pthread_t new_thread;
		pthread_create(&new_thread, NULL, accept_request, new_sock);
	}

	printf("this is httpd");
	return 0;
}
