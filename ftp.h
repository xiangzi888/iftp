/*
 * =====================================================================================
 *
 *       Filename:  ftp.h
 *
 *    Description:  与ftp有关的全局变量声明，函数声明等
 *
 *        Version:  1.0
 *        Created:  08/11/2012 01:03:52 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xiangzi888 (), xiangzi888888@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */


/*-----------------------------------------------------------------------------
 *  外部变量声明
 *-----------------------------------------------------------------------------*/
extern FILE *cin, *cout;

extern int ftpport;
extern int data;
extern int connected;
extern int code;

extern char req[];
extern char ans[];

/* 数据传输格式控制 */
extern int type;
extern int passivemode;
extern int automatic;

extern char typenm[];
extern char hostnm[];
extern char pasv[]; 

extern struct sockaddr_in cliaddr;
extern struct sockaddr_in seraddr;
extern struct sockaddr_in dataddr;

/*-----------------------------------------------------------------------------
 *  函数声明
 *-----------------------------------------------------------------------------*/
const int getftpport(void);
void lostpeer(int nu);
int setpeer(char *host);
void atlogin(void);
char *hookup (char *domain);
int getreply(void);
int command (const char *fmt, ...);
const char *onoff(int flag);



