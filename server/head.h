#ifndef _HEAD_H_
#define _HEAD_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define ERROR_EXIT(_err_msg_) do{ \
	perror(_err_msg_);\
	exit(EXIT_FAILURE);\
}while(0)

/*
 *NMAE:通用信息结构体
 */
struct message{
	char cmd;
	char name[32];
	char password[32];
	char message[32];
	char sex[16];
	int  age;
	char phone[16];
	char addr[64];
};


#define REGISTER 'R'
#define LOGIN    'L'
#define FORGET   'F'
#define EXIT     'E'
#define UPDATE   'U'
#define SEARCH   'S'
#define LEXIT    'X'

//成功返回的字符
#define SUCCESS  '#' 

//失败返回的字符
#define FAILURE  '#+1'






#endif   /*_HEAD_H_*/
