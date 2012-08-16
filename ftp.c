/*
 * =====================================================================================
 *
 *       Filename:  ftp.c
 *
 *    Description:  基本功能的实现（不包括具体命令）
 *
 *        Version:  1.0
 *        Created:  08/11/2012 10:42:54 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xiangzi888 (), xiangzi888888@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/ftp.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>


#include "ftp.h"

/*-----------------------------------------------------------------------------
 *  宏定义
 *-----------------------------------------------------------------------------*/
#define MAXNAME 20
#define MAXHOST 100


/*-----------------------------------------------------------------------------
 *  变量定义
 *-----------------------------------------------------------------------------*/
FILE *cin, *cout;

struct sockaddr_in cliaddr;
struct sockaddr_in seraddr;
struct sockaddr_in dataddr;

int data = -1;
int ftpport;
int connected = 0;
int logined = 0;
int code;

/* 数据传输格式控制 */
int type;
int curtype;	
int passivemode = 0;
int autologin = 0;
int sendport;
int pflag;

char typenm[MAXNAME];

/* 服务器主机名 */
char hostnm[MAXHOST];
char pasv[64];

/* 请求，响应标志 */
char req[] = "===>>\t";
char ans[] = "\t<<===";



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getftpport
 *  Description:  get standard ftp port 
 * =====================================================================================
 */
	int	
getftpport (void)
{
	struct servent *servinfo;

	servinfo = getservbyname("ftp", "tcp");
	if (!servinfo) {
		fprintf(stderr, "No ftp service!\n");
		exit(1);
	}

	return servinfo->s_port;
}		/* -----  end of function getftpport  ----- */



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hookup
 *  Description:  建立连接
 * =====================================================================================
 */
	char *
hookup (char *domain)
{
	struct hostent *hostp;
	int sockfd = 0;
	
	/* get host's ip seraddr */
	if (!(hostp = gethostbyname(domain))){
		fprintf(stderr, "Ftp: %s\n", domain);
		goto BAD;
	}

	/* name the socket */	
	seraddr.sin_family = hostp->h_addrtype;
    seraddr.sin_addr = *(struct in_addr *)*hostp->h_addr_list;
    seraddr.sin_port = ftpport;

#ifdef DEBUG
	printf("remote ip address: %s\n", inet_ntoa(seraddr.sin_addr));
#endif

	/* create a new socket for client */
	if ((sockfd = socket(seraddr.sin_family, SOCK_STREAM, 0)) < 0){
		perror("Ftp: socket");
		goto BAD;
	}
	
	/* Now connect our socket to the server's socket */
    if (connect(sockfd, (struct sockaddr *)&seraddr, sizeof(seraddr)) < 0){
		perror("Ftp: connect");
		goto BAD;
	}
	
	
	/* 获得客户的信息，ip，port */
	socklen_t len = sizeof(cliaddr);
	if ((getsockname(sockfd, (struct sockaddr *)&cliaddr, &len)) < 0){
		perror("Ftp: getsockname");
		goto BAD;
	}
#ifdef DEBUG
	printf("local ip: %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
#endif

	/* 连接成功 */
	cin = fdopen(sockfd, "r");
	cout= fdopen(sockfd, "w");

	if (cin == NULL || cout == NULL) {
		fprintf(stderr, "Ftp: fdopen failed.\n");
		if (cin)
			fclose(cin);
		if (cout)
			fclose(cout);
		goto BAD;
	}

	strncpy(hostnm, hostp->h_name, sizeof(hostnm));
	hostnm[sizeof(hostnm) - 1] = '\0';
	
	printf("Connected to %s\n", hostnm);

	/* read startup message from server */
	if (getreply() > 2){
		if (cin)
			fclose(cin);
		if (cout)
			fclose(cout);
		goto BAD;
	}
	return hostnm;
BAD:
	code = -1;
	if (sockfd)
		close(sockfd);
	return (char *)0;
}		/* -----  end of function hookup  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getreply
 *  Description:  获取服务器相应
 * =====================================================================================
 */
	int	
getreply (void)
{
	int c, n, dig, more;
	
	for (;;){
		dig = n = code = more = 0;

		while ((c = getc(cin)) != '\n'){
			dig++;
			if (c == EOF){
				lostpeer();
				code = 421;
				printf("421 Service not available, remote server has closed connection\n");
				fflush(stdout);
				return 4;
			}
			if (c != '\r')
				putchar(c);
			if (dig < 4 && isdigit(c))
				code = code * 10 + (c - '0');
            if (dig == 4 && c == '-'){
				more = 1;
			}
			if (n == 0)
				n = c;
		}

		/* 遇到了换行符，打印一个消息的标志 */
		printf("%s%c", ans, c);
		fflush(stdout);

		if (more)
			continue;
		return n - '0';
	}
}		/* -----  end of function getreply  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  atlogin
 *  Description:  自动登录到指定域
 * =====================================================================================
 */
	void
atlogin (void)
{
	char *user = "anonymous";
	char *pass = getlogin() ? getlogin() : "guest";
	int n;

	n = command("USER %s", user);
	if (n == CONTINUE){
		n = command("PASS %s", pass);
	}
	if (n != COMPLETE){
		fprintf(stderr, "Auto login failed.\n");
		return;
	}
	logined = 1;
}		/* -----  end of function atlogin  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  lostpeer
 *  Description:  关闭连接
 * =====================================================================================
 */
	void
lostpeer (void)
{
	return ;
}		/* -----  end of function lostpeer  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  command
 *  Description:  执行命令，格式由参数给出
 * =====================================================================================
 */
	int
command (const char *fmt, ...)
{
	va_list ap;

	if (cout == NULL){
		perror("No control connection for command");
		code = -1;
		return 0;
	}
	va_start(ap, fmt);
	vfprintf(cout, fmt, ap);

	/* 发送到客户端，显示 */
	printf("%s", req);
#ifdef DEBUG1
	printf("***%s***", fmt);
#endif
	if (strncmp("PASS ", fmt, strlen("PASS ")) == 0)
		printf("PASS ********");
	else
		vfprintf(stdout, fmt, ap);
	printf("\n");
	va_end(ap);
	fprintf(cout, "\r\n");
	fflush(cout);
	return getreply();
}		/* -----  end of function command  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  onoff
 *  Description:  
 * =====================================================================================
 */
	const char *
onoff (int flag)
{
	return flag ? "on" : "off";
}		/* -----  end of function onoff  ----- */
