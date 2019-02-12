#ifndef _DB_H_
#define _DB_H_

#include <sqlite3.h>
#include "head.h"

void db_create();
void db_u_register(int fd,void *buf);
void db_u_login(int fd,void *buf);
void db_u_forget(int fd,void *buf);
void db_u_exit(int fd,void *buf);



#endif /*_DB_H_*/
