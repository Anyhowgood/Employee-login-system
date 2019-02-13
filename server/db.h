#ifndef _DB_H_
#define _DB_H_

#include <sqlite3.h>
#include "head.h"
#include <sys/types.h>
#include <sys/socket.h>

#define CMD_SIZE 1024
void db_create(sqlite3 *db);
void db_u_register(int fd,void *buf,sqlite3 *db);
void db_u_login(int fd,void *buf,sqlite3 *db);
void db_u_forget(int fd,void *buf,sqlite3 *db);
void db_u_updata(int fd,void *buf,sqlite3 *db);
void db_u_exit(int fd,void *buf,sqlite3 *db);



#endif /*_DB_H_*/
