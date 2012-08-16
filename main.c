/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  程序入口
 *
 *        Version:  1.0
 *        Created:  08/11/2012 01:02:11 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xiangzi888 (), xiangzi888888@gmail.com
 *   Organization:  Hust
 *
 * =====================================================================================
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <arpa/telnet.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>

#include "ftp.h"
#include "cmds.h"

#define MAXLINE 200


/*-----------------------------------------------------------------------------
 *  函数声明
 *-----------------------------------------------------------------------------*/
static void cmdloop(void);	
static char *getinput(char *buf, int len);
static void makearg(char *str);


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  主函数，处理命令行参数后，尝试连接并进入主循环
 * =====================================================================================
 */
	int
main( int argc, char *argv[] )
{
	ftpport = getftpport();
	margc = 2;
	margv[0] = "open";
	margv[1] = argv[1];

	OPEN(margc, margv);

	while (1)
	{
		cmdloop();
	}

	return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  cmdloop
 *  Description:  循环执行命令
 * =====================================================================================
 */
	static void
cmdloop (void)
{
	struct cmd *c;
	int l;
	char line[MAXLINE] = "";

	for (;;) {
		if (!getinput(line, sizeof(line))) {
			QUIT();
		}
		l = strlen(line);
		if (l == 0)
			break;
		if (line[--l] == '\n') {
			if (l == 0)
				break;
			line[l] = '\0';
		} 
		else if (l == sizeof(line) - 2) {
			/* it was a line without a newline */
			printf("Sorry, input line too long\n");
			while ((l = getchar()) != '\n' && l != EOF)
				;
			break;
		} 

		makearg(line);
		c = getcmd(margv[0]);
		if (c == (struct cmd *)-1) {
			printf("?Ambiguous command\n");
			continue;
		}
		if (c == NULL) {
			printf("?Invalid command\n");
			continue;
		}
		if (c->c_conn && !connected) {
			printf("Not connected.\n");
			continue;
		}
		if (c->c_handler_1) 
			c->c_handler_1(margc, margv);
		else
			c->c_handler_0();
	}
}		/* -----  end of function cmdloop  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getinput
 *  Description:  输出提示符，并获取输入
 * =====================================================================================
 */
	static char *
getinput (char *buf, int len)
{
	printf("ftp> ");
	fflush(stdout);

	return fgets(buf, len, stdin);
}		/* -----  end of function getinput  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  makearg
 *  Description:  将字符串按空格分割为参数个数及值
 * =====================================================================================
 */
	static void
makearg(char *str)
{
	char *p;
	int i = -1;
	memset(margv, 0, sizeof(margv));

	p = strtok(str, " ");
	while (p != NULL){
		if (++i < MAXARGNUM){
			margv[i] = p;
			margc = i + 1;
		}
		p = strtok(NULL, " ");
	}
#ifdef DEBUG1
	i = 0;
	while (margv[i] && i < MAXARGNUM){
		printf("%d:%s\t", i, margv[i]);
		i++;
	}
	printf("\n");
#endif
}		/* -----  end of function makearg  ----- */

