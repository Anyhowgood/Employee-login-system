/*
 *AUTHOR:廖锴
 *TIME：2019.2.13
 *DESRIBE:用户管理系统服务器端
 *FUNC:可同时支持三个用户进行操作。最大用户等待队列为10
 */
#include "head.h"
#include "db.h"

#define BUFF_SIZE 32

//客户端等待队列最大数量
#define QUEUE_SIZE 10

//最大同时在线客户端数量
int MAX_CLIENTNUM = 3;

//数据库指针
sqlite3 *db = NULL;

/*
 *NAME:用户服务函数
 *DESRIBE:为链接上的服务端提供服务
 *ARGUME: arg:外部传入的数据 强转才可以使用
 *RETURN: 无返回值  函数结束则回收资源
 */

void *user_func(void *arg)
{	
	char exit_buff[BUFF_SIZE] = "You will exit !\n";
	struct message user_msg = {0};
	int clientfd = (int)arg;

	while(1){
		memset(&user_msg,0,sizeof(user_msg));
		recv(clientfd,&user_msg,sizeof(user_msg),0);
		if(user_msg.cmd == 0)
				break;

		printf("************************\n");
	switch(user_msg.cmd){
			case 'R':
				printf("R\n");
				db_u_register(clientfd,(void *)&user_msg,db);
				user_msg.cmd = 0;
				break;
			case 'L':
				printf("L\n");
				db_u_login(clientfd,(void *)&user_msg,db);
				user_msg.cmd = 0;
				break;
			case 'F':
				printf("F\n");
				printf("name = %s \n",user_msg.name);
				printf("message = %s \n",user_msg.message);
				db_u_forget(clientfd,(void *)&user_msg,db);
				user_msg.cmd = 0;
				break;
			case 'U':
				printf("U\n");
				printf("name = %s \n",user_msg.name);
				printf("newpassword = %s \n",user_msg.newpassword);
				db_u_updata(clientfd,(void *)&user_msg,db);
				user_msg.cmd = 0;
				break;
			case 'E':
				//db_u_exit();
				goto u_exit;
			}
		printf("************************\n");
	}
	

u_exit:
	send(clientfd,exit_buff,BUFF_SIZE,0);

	MAX_CLIENTNUM++;
	pthread_exit(NULL);
}

int sock_init(const char *arv[])
{
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		ERROR_EXIT("socket");
	}
	
	struct sockaddr_in SERVER,CLIENT;
	socklen_t addr_len = 0;

	SERVER.sin_family = AF_INET;
	SERVER.sin_port = htons(atoi(arv[2]));
	SERVER.sin_addr.s_addr = inet_addr(arv[1]);
	addr_len = sizeof(SERVER);

	int ret = bind(sockfd,(const struct sockaddr *)&SERVER,addr_len);
	if(ret < 0){
		ERROR_EXIT("bind");
	}
	//设置IP端口可重复使用
	int on = 1;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(const char *)&on,sizeof(on));
	return sockfd;
}

int main(int argc, const char *argv[])
{
	if(argc != 3){
		printf("Please input 3 arguments! \n");
		exit(1);
	}

	struct sockaddr_in CLIENT;
	socklen_t addr_len = 0;
	//套接字初始化
	int sockfd = sock_init(argv);
	/******************************/
	//创建数据库或者读取数据库
	if(sqlite3_open("./people.db",&db) != SQLITE_OK){
		ERROR_EXIT("sqlite3_open");
	}
	db_create(db);
	printf("can use datebase !\n");
	/******************************/
	int ret = listen(sockfd,10);
	if(ret < 0){
		ERROR_EXIT("listen");
	}
	printf("***************************\n");
	printf("listen sockfd in running!\n");
	printf("***************************\n");
	
	/**************客户端等待队列***************/
	int ClienetQueue[QUEUE_SIZE] = {0};
	int QUEUE_NUM = 0;
	char lister[] = {"The line is full,Please wait!\n"};
	/******************************************/
	//连接成功的套接字
	int clientfd;
	while(1){
		clientfd = accept(sockfd,(struct sockaddr *)&CLIENT,&addr_len);
		if(-1 == clientfd){
			ERROR_EXIT("accept");
		}
		printf("The client ip: %s connect\n",inet_ntoa(CLIENT.sin_addr));
		
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
					int ret = pthread_create(&t,NULL,user_func,(void *)ClienetQueue[QUEUE_NUM]);
					if(ret != 0){
						ERROR_EXIT("pthread_create");
					}
					QUEUE_NUM--;
				}
				else{
					int ret = pthread_create(&t,NULL,user_func,(void *)clientfd);
					if(ret != 0){
						ERROR_EXIT("pthread_create");
					}
				}

				pthread_detach(t);
				MAX_CLIENTNUM--;
			}
			
	}

	return 0;
}		
