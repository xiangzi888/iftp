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


#include "ftp.h"

/*-----------------------------------------------------------------------------
 *  宏定义
 *-----------------------------------------------------------------------------*/
#define MAXNAME 20
#define MAXHOST 200


/*-----------------------------------------------------------------------------
 *  变量定义
 *-----------------------------------------------------------------------------*/
FILE *cin, *cout;

int ftpport;
int connected = 0;
int logined;
int code;

/* 数据传输格式控制 */
int form;
int type;
int curtype;	//???
int mode;
int stru;
int bytesize;

char formname[MAXNAME];
char typename[MAXNAME];
char modename[MAXNAME];
char structname[MAXNAME];
char bytename[MAXNAME];

/* 服务器主机名 */
char hostname[MAXHOST];



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
		fprintf(stderr, "no ftp service!\n");
		exit(1);
	}

	return servinfo->s_port;
}		/* -----  end of function getftpport  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  setpeer
 *  Description:  连接到服务器，若未登录则执行登录操作
 * =====================================================================================
 */
	void
setpeer (char *domain)
{
	char *host;

	if (connected) {
		printf("Already connected to %s, use close first.\n", hostname);
		return;
	}
	host = hookup(domain);
	if (host){
		connected = 1;

		/*-----------------------------------------------------------------------------
		 *  Set up defaults for FTP.
		 *-----------------------------------------------------------------------------*/
		strcpy(typename, "ascii"), type = TYPE_A, curtype = TYPE_A;
		strcpy(formname, "non-print"), form = FORM_N;
		strcpy(modename, "stream"), mode = MODE_S;
		strcpy(structname, "file"), stru = STRU_F;
		strcpy(bytename, "8"), bytesize = 8;	//???
		if (!logined)
			login(host);
	}
}		/* -----  end of function setpeer  ----- */


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
	struct sockaddr_in seraddr;
	int sockfd = 0;
	
	/* get host's ip seraddr */
	if (!(hostp = gethostbyname(domain))){
		fprintf(stderr, "ftp: %s\n", domain);
		goto BAD;
	}

	/* name the socket */	
	seraddr.sin_family = hostp->h_addrtype;
    seraddr.sin_addr = *(struct in_addr *)*hostp->h_addr_list;
    seraddr.sin_port = ftpport;

#ifdef DEBUG
	printf("%s\n", inet_ntoa(seraddr.sin_addr));
#endif

	/* create a new socket for client */
	if ((sockfd = socket(seraddr.sin_family, SOCK_STREAM, 0)) < 0){
		perror("ftp: socket");
		goto BAD;
	}
	
	/* Now connect our socket to the server's socket */
    if (connect(sockfd, (struct sockaddr *)&seraddr, sizeof(seraddr)) < 0){
		perror("ftp: connect");
		goto BAD;
	}
	
	/* 连接成功 */
#ifdef DEBUG
	struct sockaddr_in cliaddr;
	socklen_t len = sizeof(cliaddr);
	
	/* 获得客户的信息，ip，port */
	if ((getsockname(sockfd, (struct sockaddr *)&cliaddr, &len)) < 0){
		perror("ftp: getsockname");
		goto BAD;
	}
	printf("%s\n", inet_ntoa(cliaddr.sin_addr));
	printf("%d\n", ntohs(cliaddr.sin_port));
#endif

	cin = fdopen(sockfd, "r");
	cout= fdopen(sockfd, "w");
	if (cin == NULL || cout == NULL) {
		fprintf(stderr, "ftp: fdopen failed.\n");
		if (cin)
			fclose(cin);
		if (cout)
			fclose(cout);
		goto BAD;
	}

	strncpy(hostname, hostp->h_name, sizeof(hostname));
	hostname[sizeof(hostname) - 1] = '\0';
	
	printf("Connected to %s\n", hostname);

	/* read startup message from server */
	if (getreply() > 2){
		if (cin)
			fclose(cin);
		if (cout)
			fclose(cout);
		goto BAD;
	}
	return hostname;
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
		/* 遇到了换行符 */
		putchar(c);
		fflush(stdout);

		if (more)
			continue;
		return n - '0';
	}
}		/* -----  end of function getreply  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  login
 *  Description:  
 * =====================================================================================
 */
	void
login (char *domain)
{
	return;
}		/* -----  end of function login  ----- */

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
