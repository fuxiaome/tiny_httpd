#include"demo_client.h"

void usage(const char *msg)
{
	printf("Usage:%s [IP] [PORT] [CMD]\n");
}

int main(int argc, char *argv[])
{
	if( argc != 3){
		usage(argv[0]);
		exit(1);
	}
	//创建客户端Socket
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1){
		perror("socket");
		exit(1);
	}

	//填写服务器端的相关信息
	short port = atoi(argv[2]);
	struct sockaddr_in remote_server;
	remote_server.sin_family = AF_INET;
	remote_server.sin_port = htons(port);
	remote_server.sin_addr.s_addr = inet_addr(argv[1]);

	//与服务器建立连接
	socklen_t remote_len = sizeof(remote_server);
	if( connect(sock, (struct sockaddr*)&remote_server, remote_len) < 0 ){
		perror("connect");
		exit(2);
	}

	const char *cmd = argv[3];//CMD表示向服务器端发送的请求
	int len = strlen(cmd);
	ssize_t size = send(sock, cmd, len, 0);//send(sock,void *buf,size,flag)
	if(size <= 0){
		perror("send");
		close(sock);
		return 1;
	}

	char buf[1024];
	memset(buf, 0, sizeof(buf));
	while( recv(sock, buf, sizeof(buf)-1, 0) > 0 ){//接收服务器端的反馈信息
		printf("%s", buf);
		memset(buf, 0, sizeof(buf));
	}
	printf("\n");
	close(sock);
	return 0;
}
