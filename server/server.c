#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#include "head.h"

#define QUEUE_SIZE 10
#define BUFF_SIZE 32

int MAX_CLIENTNUM = 3;
/*
 *NAME:用户服务函数
 *DESRIBE:为链接上的服务端提供服务
 *ARGUME: arg:外部传入的数据 强转才可以使用
 *RETURN: 无返回值  函数结束则回收资源
 */
void *user_func(void *arg)
{	
//	char recv_buff[BUFF_SIZE] = {0};
//	char send_buff[BUFF_SIZE] = {0};
	char exit_buff[BUFF_SIZE] = "You will exit !\n";
	struct message user_msg;
	int clientfd = (int)arg;

	while(1){
		recv(clientfd,(struct message *)&user_msg,sizeof(user_msg),0);
		
			switch(user_msg.cmd){
			case 'R':
				//db_u_register(int fd,void *buf);
				printf("The name = %s\n",user_msg.name);
				user_msg.cmd = 0;
				break;
			case 'L':
				//db_u_login(int fd,void *buf);
				user_msg.cmd = 0;
				break;
			case 'F':
				//db_u_forget(int fd,void *buf);
				user_msg.cmd = 0;
				break;
			case 'E':
				//db_u_exit();
				goto u_exit;
		//		break;
		//	default:
				
			}
			if(user_msg.cmd == 0)
				break;


	}

u_exit:
	send(clientfd,exit_buff,BUFF_SIZE,0);

	MAX_CLIENTNUM++;
	pthread_exit(NULL);
}

int main(int argc, const char *argv[])
{

	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		ERROR_EXIT("socket");
	}
		
	struct sockaddr_in SERVER,CLIENT;
	socklen_t addr_len = 0;

	SERVER.sin_family = AF_INET;
	SERVER.sin_port = htons(atoi(argv[2]));
	SERVER.sin_addr.s_addr = inet_addr(argv[1]);
	addr_len = sizeof(SERVER);

	int ret = bind(sockfd,(struct sockaddr *)&SERVER,addr_len);
	if(ret < 0){
		ERROR_EXIT("bind");
	}

	listen(sockfd,10);
	printf("listen sockfd in running!\n");

	int ClienetQueue[QUEUE_SIZE] = {0};
	int QUEUE_NUM = 0;
	char lister[] = {"The line is full,Please wait!\n"};
	while(1){
		int clientfd = accept(sockfd,(struct sockaddr *)&CLIENT,&addr_len);
		printf("The client ip: %s connect\n",inet_ntoa(CLIENT.sin_addr));
		
		if(clientfd > 0){
			if(MAX_CLIENTNUM <= 0){
				write(clientfd,lister,sizeof(lister));
				ClienetQueue[QUEUE_NUM] = clientfd;
				if(QUEUE_NUM < QUEUE_SIZE){
					QUEUE_NUM++;
				}
			}

			if(MAX_CLIENTNUM > 0){
				pthread_t t;
				if(QUEUE_NUM > 0){
					pthread_create(&t,NULL,user_func,(void *)ClienetQueue[QUEUE_NUM]);
					QUEUE_NUM--;
				}
				else
					pthread_create(&t,NULL,user_func,(void *)clientfd);

				pthread_detach(t);
				MAX_CLIENTNUM--;
			}
		}
	}

	return 0;
}		
