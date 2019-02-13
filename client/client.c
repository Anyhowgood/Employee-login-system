/*
 * personnel mannagement/clint
 * hcl 2019/2/11
 * v 0.0
 */

#include "head.h"
#define BUFSIZE 64
struct sockaddr_in peer_addr;
socklen_t peer_addrlen = sizeof(peer_addr);

int main(int argc, char* argv[])
{
    /*person  message */
	struct message clint_message = {0};
	/*cmd use in external switch*/
    int cmd = 0;
	/*cmd use in login switch*/
	int in_cmd = 0;
    char buf[BUFSIZE] = {};	
	int socketfd;
	socketfd = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == socketfd)
	{
		perror("sockt");
	}
	peer_addr.sin_family = AF_INET;
//	peer_addr.sin_port   = htons(8888);
//	peer_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	peer_addr.sin_port   = htons(60000);
	peer_addr.sin_addr.s_addr = inet_addr("192.168.1.123");
	if(-1 == connect(socketfd,(const struct sockaddr *)&peer_addr,peer_addrlen))
	{
		perror("connect");
		return -1;
	}
	printf("on line.....\n");
	printf("\n");
	
	while(1)
	{
		printf("******************************************************\n");
		printf("*1:register   2:login   3:forget password    4:exit*\n");
		printf("******************************************************\n");
		printf("please choose:");
		scanf("%d",&cmd);
		/*deleat scanf remain char*/
		gets(buf);
		switch(cmd)
		{
			case 1:
				memset(&clint_message,0,sizeof(clint_message));

			    /*fill person message struct */
				clint_message.cmd = REGISTER;
				printf("intput your name:");
				gets(clint_message.name);
				
				printf("intput your password:");
				gets(clint_message.password);
				
				printf("intput your message:");
				gets(clint_message.message);
				
				printf("intput your sex:");
				gets(clint_message.sex);
				
				printf("intput your age:");
				scanf("%d",&clint_message.age);
				/*deleat scanf remain char*/
				gets(buf);

				printf("intput your phone:");
				gets(clint_message.phone);
				
				printf("intput your addr:");
				gets(clint_message.addr);
				
				/*send already filled person message struct to server*/
				send(socketfd,&clint_message,sizeof(clint_message),0);
			    memset(buf,0,sizeof(buf));
				
				recv(socketfd,buf,sizeof(buf),0);
				if('#' == buf[0])
				{
					printf("register success\n");
					break;
				}
				else if('A' == buf[0])
				{
					printf("name exist\n");
				    break;

				}
				else
				{
				    printf("register error \n");
                 	break;
				}
				
			case 2:
				memset(&clint_message,0,sizeof(clint_message));

			    /*fill person message struct */
				clint_message.cmd = LOGIN;
				printf("intput your name:");
				gets(clint_message.name);
				
				printf("intput your password:");
				gets(clint_message.password);
				
				/*send already filled person message struct to server*/
				send(socketfd,&clint_message,sizeof(clint_message),0);
			    memset(buf,0,sizeof(buf));
				
				recv(socketfd,buf,sizeof(buf),0);
				if('#' != buf[0])
				{
					printf("name or password error ! \n");
                    break;
					
				} 
                printf("login success ! \n");
                while(1)
                {
					printf("******************************************************\n");
		        	printf("*1:update   2:search   3:exit*\n");
		        	printf("******************************************************\n");
		        	printf("please choose:");
					scanf("%d",&in_cmd);
		       		/*deleat scanf remain char*/
		        	gets(buf);   
				  	switch(in_cmd)
				  	{
					case 1:
					memset(&clint_message,0,sizeof(clint_message));

			    	/*fill person message struct */
					clint_message.cmd = UPDATE;
					printf("intput your name:");
					gets(clint_message.name);
				
					printf("intput your newpassword:");
					gets(clint_message.newpassword);
				
					/*send already filled person message struct to server*/
					send(socketfd,&clint_message,sizeof(clint_message),0);
			    	memset(buf,0,sizeof(buf));
				
					recv(socketfd,buf,sizeof(buf),0);
					printf("%s \n",buf);
					if('#' == buf[0])
					{
						printf("update success\n");
						goto login_exit;
					}
					else
					{
					    printf("update error \n");
                	 	break;
					}
					case 2:
					    printf("the Method not implemented \n");
						break;
					case 3:
						printf("exit....!\n");
						goto login_exit;
					default:
					    printf("cmd erro !\n");
					    break;
					} 
				}
                login_exit:
				break;
				
			case 3:
				memset(&clint_message,0,sizeof(clint_message));

			    /*fill person message struct */
				clint_message.cmd = FORGET;
				printf("intput your name:");
				gets(clint_message.name);
				
				printf("intput your message:");
				gets(clint_message.message);
				
				/*send already filled person message struct to server*/
				send(socketfd,&clint_message,sizeof(clint_message),0);
			    memset(buf,0,sizeof(buf));
				
				recv(socketfd,buf,sizeof(buf),0);
				printf("%s\n",buf);
				if('#' == buf[0])
				{
					printf("forgetpassword success\n");
					printf("the password is %s\n",(char *)&buf[1]);
					break;
				}
				else if('C' == buf[0])
				{
					printf("name or message error \n");
					break;
				}
				else
				{
					printf("forgetpassword error \n");
                	break;
				}
			case 4:
				printf("the program exiting....\n");
				goto exit;
			default:
				printf("cmd erro!\n");
			    break;
		}

		
	}
	exit:
	return 0;
}
