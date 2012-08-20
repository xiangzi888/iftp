/*
 * =====================================================================================
 *
 *       Filename:  cmds.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/11/2012 01:02:37 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xiangzi888 (), xiangzi888888@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

/* 最多可分割20各参数 */
#define MAXARGNUM 20 


/*-----------------------------------------------------------------------------
 *  命令函数声明
 *-----------------------------------------------------------------------------*/
struct cmd *getcmd(char *name);
void recvreq(const char *cmd, char *local, char *remote, const char *lmod);
int initconn(void);
FILE *dataconn(const char *lmode);
char *protect(char *name);

void QUIT(void);
void CLOSE(void);
void PWD(void);
void OPEN(int argc, char *argv[]);
void USER(int argc, char *argv[]);
void SYSTEM(void);
void ASCII(void);
void BINARY(void);
void PASSIVE(void);
void AUTOMATIC(void);
void TYPE(int argc, char *argv[]);
void GET(int argc, char *argv[]);
void LS(int argc, char *argv[]);
void GET(int argc, char *argv[]);


/*-----------------------------------------------------------------------------
 *  变量参数
 *-----------------------------------------------------------------------------*/
extern int margc;
extern char *margv[MAXARGNUM];

struct cmd {
	const char *c_name;	/* name of command */
	const char *c_help;	/* help string */
	char c_conn;		/* must be connected to use command */

    /* Exactly one of these should be non-NULL, function to call */
	void (*c_handler_1)(int argc, char *argv[]); 
	void (*c_handler_0)(void);
};
