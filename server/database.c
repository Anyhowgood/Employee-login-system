#include "db.h"

void db_create(sqlite3 *db)
{
	char *create_command = "create table if not exists user (name,password,\
								message,sex,age,phone,addr);";

	char *err;
	int ret;
	//执行SQL语句方法一，
	//其中第三个参数为回调函数指针，当查询有结果时会被回调
	ret = sqlite3_exec(db, create_command, NULL, NULL, &err);
	if(ret != SQLITE_OK){
		printf("Fail : %s\n",err);
		exit(1);
	}

	printf("You  can operate the  database !\n");
	return ;
}
void db_u_register(int fd,void *buf,sqlite3 *db)
{
	struct message *user_msg = (struct message *)buf;
	//检测数据是否为空
	if(user_msg->name == NULL){
		printf("data is empty!\n");
		exit(1);
	}

	printf("%s \n",user_msg->name);
	printf("%s \n",user_msg->password);
	printf("%s \n",user_msg->message);
	printf("%s \n",user_msg->sex);
	printf("%d \n",user_msg->age);
	printf("%s \n",user_msg->phone);
	printf("%s \n",user_msg->addr);

	//指令存储数组
	char register_succee[] = "You info succeed register!\n";
	char buff[CMD_SIZE] = {0};
//	char *register_cmd = "insert into user values (name,password,\
//						  	message,sex,age,phone,addr);"

	sprintf(buff,"insert into user values('%s','%s','%s','%s','%d',\
					'%s','%s');",user_msg->name,user_msg->password,\
					user_msg->message,user_msg->sex,user_msg->age,\
					user_msg->phone,user_msg->addr);

	int ret = sqlite3_exec(db,buff,NULL,NULL,NULL);
	if(ret != SQLITE_OK){
		ERROR_EXIT("sq insert");
	}

//	send(fd,register_succee,sizeof(register_succee),0);
//	recv(fd,buff,CMD_SIZE,0);

}
void db_u_login(int fd,void *buf)
{

}
void db_u_forget(int fd,void *buf)
{

}
void db_u_exit(int fd,void *buf)
{

}
