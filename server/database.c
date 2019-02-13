#include "db.h"

static int db_exist(const void *buf,sqlite3 *db);
static int db_mes_exist(const void *buf,sqlite3 *db);

//从数据库中读取的当前用户密码
static char mes_pass[64] = {0};

/*
 *NAME:数据库创建函数
 *DESRIBE：如果没有数据库文件，则创建一个数据库文件
 *ARGUME： db:数据库指针
 *RETURN： 无
 */
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
/*
 *NAME: 用户注册函数
 *DESRIBE：将用户信息写入数据库中
 *ARGUME：fd:网络句柄  buf :外部数据  db:数据库指针
 *RETURN： 无
 */
void db_u_register(int fd,void *buf,sqlite3 *db)
{
	struct message *user_msg = (struct message *)buf;
	//检测数据是否为空
	if(user_msg->name == NULL){
		printf("data is empty!\n");
		exit(1);
	}

	//指令存储数组
	char register_succee[] = "You info succeed register!\n";
	char user_exist[] = "The name is exist Please again a name !\n";
	char buff[CMD_SIZE] = {0};
	
	//用户名存在检测
	int ret = db_exist(buf,db);
	if((ret == 2)){
		printf("name is exist!\n");
		//	ERROR_EXIT("db_exist");
		send(fd,NAME_EXIST,sizeof(NAME_EXIST),0);
		//recv(fd,buff,CMD_SIZE,0);
	}
	else{
		sprintf(buff,"insert into user values('%s','%s','%s','%s','%d',\
			'%s','%s');",user_msg->name,user_msg->password,\
			user_msg->message,user_msg->sex,user_msg->age,\
			user_msg->phone,user_msg->addr);

		ret = sqlite3_exec(db,buff,NULL,NULL,NULL);
		if(ret != SQLITE_OK){
			ERROR_EXIT("sq insert");
		}

		send(fd,R_SUCCEE,sizeof(R_SUCCEE),0);
		//recv(fd,buff,CMD_SIZE,0);
	}

}

/*
 *NAME:    用户登录
 *DESRIBE：对比用户名 密码后登录成功  发送成功建值對
 *ARGUME：fd:网络句柄  buf :外部数据  db:数据库指针
 *RETURN：无
 */
void db_u_login(int fd,void *buf,sqlite3 *db)
{
	char buff[CMD_SIZE] = {0};
	struct message *user_msg = (struct message *)buf;
	//用户名,密码存在检测
	int ret = db_exist(buf,db);
	if((ret != 1)){
		printf("name is exist!\n");
		send(fd,NAME_OR_PASS_F,sizeof(NAME_OR_PASS_F),0);
		//recv(fd,buff,CMD_SIZE,0);
	}
	else{
		send(fd,L_SUCCEE,sizeof(L_SUCCEE),0);
	}

	//recv(fd,buff,CMD_SIZE,0);
}
/*
 *NAME:    忘记密码提取数据库中密码
 *DESRIBE：对比用户信息无误后发送用户密码
 *ARGUME：buf :外部数据  db:数据库指针
 *RETURN：只有用户名存在返回2  都存在返回1  都不存在返回0
 */
void db_u_forget(int fd,void *buf,sqlite3 *db)
{
	char buff[CMD_SIZE] = {0};
	struct message *user_msg = (struct message *)buf;

	int ret = db_mes_exist(buf,db);
	if(ret == 1){
		send(fd,mes_pass,sizeof(mes_pass),0);
	}
	else{
		printf("message is fail!\n");
		send(fd,MES_F,sizeof(MES_F),0);
	}
	printf("ret = %d\n",ret);

}

/*
 *NAME:    修改用户信息
 *DESRIBE：对比用户名 密码后修改数据库中用户信息
 *ARGUME：fd:网络句柄  buf :外部数据  db:数据库指针
 *RETURN：无
 */
void db_u_updata(int fd,void *buf,sqlite3 *db)
{
	char buff[CMD_SIZE] = {0};

	int ret = db_exist(buf,db);
	if(ret == 2){

		struct message *user_msg = (struct message *)buf;


		sprintf(buff,"update user set password = '%s' where name = '%s';\
				",user_msg->newpassword,user_msg->name);

		int ret = sqlite3_exec(db, buff,NULL, NULL,NULL);
		if(ret != SQLITE_OK)
		{
			perror("sq change");
		}
		send(fd,U_SUCCEE,sizeof(U_SUCCEE),0);
		printf("update succeed\n");
	}
	else{
		send(fd,NAME_OR_PASS_F,sizeof(NAME_OR_PASS_F),0);
		printf("update fail\n");
	}
}

void db_u_exit(int fd,void *buf,sqlite3 *db)
{

}
/*
 *NAME:数据库查询用户 帐号 密码 是否存在
 *DESRIBE：對传入的数据在数据库中比对
 *ARGUME：buf :外部数据  db:数据库指针
 *RETURN：只有用户名存在返回2  都存在返回1  都不存在返回0
 */
static int db_exist(const void *buf,sqlite3 *db)
{	

	struct message *user_msg = (struct message *)buf;

	char **resultp;//存储位置
	int nrow;//行
	int ncolumn;//列
	char buff[CMD_SIZE] = "select * from user;";

	int exist_flag = 0;

	sqlite3_get_table(db, buff,  &resultp,  &nrow,  &ncolumn, NULL);

	int i,j;
	for(i = 0;i < nrow*ncolumn; i++)
	{
		for(j = 0;j < ncolumn ; j++)
		{
			if(strcmp(resultp[i+j],user_msg->name) == 0){
					exist_flag = 2;
				if(strcmp(resultp[i+j+1],user_msg->password) == 0){
					exist_flag = 1;
				}
			}

		}

	}

	return exist_flag;
}
/*
 *NAME:    用户名与message存在查询函数
 *DESRIBE：對传入的数据在数据库中比对
 *ARGUME：buf :外部数据  db:数据库指针
 *RETURN：只有用户名与mes匹配返回 1  用户名没错返回 2  都错了返回0
 */
static int db_mes_exist(const void *buf,sqlite3 *db)
{	

	struct message *user_msg = (struct message *)buf;

	char **resultp;//存储位置
	int nrow;//行
	int ncolumn;//列
	char buff[CMD_SIZE] = "select * from user;";

	int exist_flag = 0;

	sqlite3_get_table(db, buff,  &resultp,  &nrow,  &ncolumn, NULL);

	int i,j;
	for(i = 0;i < nrow*ncolumn; i++)
	{
		for(j = 0;j < ncolumn ; j++)
		{
			if(strcmp(resultp[i+j],user_msg->name) == 0){
					exist_flag = 2;
				if(strcmp(resultp[i+j+2],user_msg->message) == 0){
					exist_flag = 1;
				//	strcpy(mes_pass,resultp[i+j+1]);
					sprintf(mes_pass,"#%s",resultp[i+j+1]);
				}
			}

		}

	}

	return exist_flag;
}
