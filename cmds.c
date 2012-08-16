/*
 * =====================================================================================
 *
 *       Filename:  cmds.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/11/2012 01:03:38 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xiangzi888 (), xiangzi888888@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/ftp.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <ctype.h>
#include <limits.h>	/* for PATH_MAX */
#include <time.h>
#include <string.h>
#include <unistd.h>

#include "ftp.h"
#include "cmds.h"

#define MAXLOGIN	80

/*-----------------------------------------------------------------------------
 *  变量参数
 *-----------------------------------------------------------------------------*/
int margc;
char *margv[MAXARGNUM];

struct types{
	const char *t_name;
	const char *t_mode;
	int t_type;
} types[] = {
	{ "ascii",	"A",	TYPE_A},
	{ "binary",	"I",	TYPE_I},
	{ "image",	"I",	TYPE_I},
	{ NULL, 	NULL, 	0}
};

struct cmd cmdtab[] = {
	{ "quit",	"HQUIT",	0,	NULL,	QUIT},
	{ "bye",	"HQUIT",	0,	NULL,	QUIT},
	{ "ls",		"HLS",		1,	LS,		NULL},
	{ "get",	"HGET",		1,	GET,	NULL},
	{ "pwd",	"HPWD",		1,	NULL,	PWD},
	{ "close",	"HCLOSE",	1,	NULL,	CLOSE},
	{ "ascii",	"HASCII",	1,	NULL,	ASCII},
	{ "binary",	"HBINARY",	1,	NULL,	BINARY},
	{ "image",	"HBINARY",	1,	NULL,	BINARY},
	{ "open",	"HOPEN",	0,	OPEN,	NULL},
	{ "user",	"HUSER",	1,	USER,	NULL},
	{ "type",	"HTYPE",	1,	TYPE,	NULL},
	{ "system",	"HSYSTEM",	1,	NULL,	SYSTEM},
};

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getcmd
 *  Description:  
 * =====================================================================================
 */
	struct cmd *
getcmd (char *name)
{
	struct cmd *c, *found = 0;
	int nmatches = 0;

	for (c = cmdtab; c->c_name != NULL; c++) {
		if (strcmp(name, c->c_name) == 0){
			/* exactly matches */
			return c;
		}
		if (strncmp(name, c->c_name, strlen(name)) == 0){
			found = c;
			nmatches++;
		}
	}
	if (nmatches > 1)
		found = (struct cmd *)-1;
	return found;
}		/* -----  end of function getcmd  ----- */



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  QUIT
 *  Description:  
 * =====================================================================================
 */
	void
QUIT (void)
{	
	if (connected)
		CLOSE();
	exit(0);
}		/* -----  end of function QUIT  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  LS
 *  Description:  
 * =====================================================================================
 */
	void
LS (int argc, char *argv[])
{
	
	const char *c;
	if (argc > 3){
		printf("Usage: %s [remote-dir] [local-file]\n", argv[0]);	
		code = -1;
		return;
	}

	if (argc < 2) {
		argc++, argv[1] = ".";
	}
	if (argc < 3) {
		argc++, argv[2] = "-";
	}

	c = (argv[0][0] == 'n' ? "NLST" : "LIST");
	recvreq(c, argv[2], argv[1], "w");
}		/* -----  end of function LS----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  PWD
 *  Description:  
 * =====================================================================================
 */
	void
PWD (void)
{
	command("PWD");
}		/* -----  end of function PWD  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  CLOSE
 *  Description:  
 * =====================================================================================
 */
	void
CLOSE (void)
{	
	command("QUIT");
	if (cout) {
		(void) fclose(cout);
	}
	cout = NULL;
	connected = 0;
	data = -1;
}		/* -----  end of function CLOSE  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  OPEN
 *  Description:  连接到服务器，若未登录则执行登录操作
 * =====================================================================================
 */
	void
OPEN (int argc, char *argv[])
{
	if (argc != 2){
		printf("Usage: %s hostname\n", argv[0]);
		return;
	}
	if (connected) {
		printf("Already connected to %s, use close first.\n", hostnm);
		return;
	}
	if (hookup(argv[1]))
		connected = 1;

		/*-----------------------------------------------------------------------------
		 *  Set up defaults for FTP.
		 *-----------------------------------------------------------------------------*/
		strcpy(typenm, "ascii"), type = TYPE_A, curtype = TYPE_A;

		if (autologin)
			atlogin();
}		/* -----  end of function OPEN  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  GET
 *  Description:  
 * =====================================================================================
 */
	void
GET (int argc, char *argv[])
{
	return ;
}		/* -----  end of function GET  ----- */



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  recvreq
 *  Description:  
 * =====================================================================================
 */
	void
recvreq(const char *cmd, char *local, char *remote, const char *lmode)
{
	FILE *fout, *din = 0;
	long bytes = 0;
	int c;

	if (initconn()) {
		code = -1;
		return;
	}
	if (command("%s %s", cmd, remote) != PRELIM) {
		return;
	}
	din = dataconn("r");
	if (din == NULL)
		goto ABORT;
	if (strcmp(local, "-") == 0)
		fout = stdout;
	else {
		fout = fopen(local, lmode);
		if (fout == NULL) {
			fprintf(stderr, "Local: %s: %s\n", local, strerror(errno));
			goto ABORT;
		}
	}
	switch (curtype) {
		case TYPE_I:
		case TYPE_A:
			while ((c = getc(din)) != EOF) {
				putc(c, fout);
				bytes++;
			}
			break;
		default:
			break;
	}
	fclose(din);
	data = -1;
	getreply();
	return;

ABORT:
	/* abort using RFC959 recommended IP,SYNC sequence  */
	//abortr(din); ???
	code = -1;
	if (din) {
		fclose(din);
	}
	if (data >= 0) {
		/* if it just got closed with din, again won't hurt */
		close(data);
		data = -1;
	}
	if (bytes > 0)
		printf("received %ld bytes\n", bytes);
}		/* -----  end of function recvreq  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  initconn
 *  Description:  
 * =====================================================================================
 */
	int	
initconn (void)
{
	char *p, *a;
	int result, tmpno = 0, on = 1;
	socklen_t len;
	u_long a1,a2,a3,a4,p1,p2;

	if (passivemode) {
		data = socket(AF_INET, SOCK_STREAM, 0);
		if (data < 0) {
			perror("Ftp: socket");
			return 1;
		}
		if (command("PASV") != COMPLETE) {
			printf("Passive mode refused.\n");
			return 1;
		}

		/*
		 * What we've got at this point is a string of comma separated
		 * one-byte unsigned integer values, separated by commas.
		 * The first four are the an IP address. The fifth is the MSB
		 * of the port number, the sixth is the LSB. From that we'll
		 * prepare a sockaddr_in.
		 */

		if (sscanf(pasv,"%ld,%ld,%ld,%ld,%ld,%ld",
			&a1,&a2,&a3,&a4,&p1,&p2) != 6) {
			printf("Passive mode address scan failure. Shouldn't happen!\n");
			return 1;
		}

		dataddr.sin_family = AF_INET;
		dataddr.sin_addr.s_addr = htonl((a1 << 24) | (a2 << 16) | (a3 << 8) | a4);
		dataddr.sin_port = htons((p1 << 8) | p2);

		if (connect(data, (struct sockaddr *)&dataddr, sizeof(dataddr))<0) {
			perror("Ftp: connect");
			return 1;
		}
		return 0;
	}
NOPORT:
	dataddr = cliaddr;
	if (sendport)
		dataddr.sin_port = 0;	/* let system pick one */ 
	if (data != -1)
		close(data);
	data = socket(AF_INET, SOCK_STREAM, 0);
	if (data < 0) {
		perror("Ftp: socket");
		if (tmpno)
			sendport = 1;
		return 1;
	}
	if (!sendport)
		if (setsockopt(data, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof (on)) < 0) {
			perror("Ftp: setsockopt (reuse address)");
			goto BAD;
		}
	if (bind(data, (struct sockaddr *)&dataddr, sizeof (dataddr)) < 0) {
		perror("Ftp: bind");
		goto BAD;
	}
	/* 获得内核分配的临时端口 */
	len = sizeof (dataddr);
	if (getsockname(data, (struct sockaddr *)&dataddr, &len) < 0) {
		perror("Ftp: getsockname");
		goto BAD;
	}
	if (listen(data, 1) < 0)
		perror("Ftp: listen");
	if (sendport) {
		a = (char *)&dataddr.sin_addr;
		p = (char *)&dataddr.sin_port;
#define	UC(b)	(((int)b) & 0xff)
		result =
		    command("PORT %d,%d,%d,%d,%d,%d",
		      UC(a[0]), UC(a[1]), UC(a[2]), UC(a[3]),
		      UC(p[0]), UC(p[1]));
		if (result == ERROR && sendport == -1) {
			sendport = 0;
			tmpno = 1;
			goto NOPORT;
		}
		return (result != COMPLETE);
	}
	if (tmpno)
		sendport = 1;
	return 0;
BAD:
	close(data), data = -1;
	if (tmpno)
		sendport = 1;
	return 1;
}		/* -----  end of function initconn  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  PASV
 *  Description:  
 * =====================================================================================
 */
	void
PASV (void)
{
	passivemode = !passivemode;
	printf("Passive mode %s.\n", onoff(passivemode));
	code = passivemode;
}		/* -----  end of function PASV  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  PORT
 *  Description:  
 * =====================================================================================
 */
	void
PORT (void)
{
	sendport = !sendport;
	printf("Use of PORT cmds %s.\n", onoff(sendport));
	code = sendport;
}		/* -----  end of function PORT  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  dataconn
 *  Description:  
 * =====================================================================================
 */
	FILE *
dataconn(const char *lmode)
{
	struct sockaddr_in from;
	int s;
	socklen_t fromlen = sizeof(from);

	if (passivemode)
		return (fdopen(data, lmode));

	s = accept(data, (struct sockaddr *) &from, &fromlen);
	if (s < 0) {
		perror("Ftp: accept");
		close(data), data = -1;
		return (NULL);
	}
	close(data);
	data = s;
	return fdopen(data, lmode);
}		/* -----  end of function dataconn  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  settype
 *  Description:  
 * =====================================================================================
 */
	void
settype(const char *thetype)
{
	struct types *p;

	for (p = types; p->t_name; p++)
		if (strcmp(thetype, p->t_name) == 0)
			break;
	if (p->t_name == 0) {
		printf("Unknown mode: %s\n", thetype);
		code = -1;
		return;
	}
	if (command("TYPE %s", p->t_mode) == COMPLETE) {
		strcpy(typenm, p->t_name);
		curtype = type = p->t_type;
	}
}		/* -----  end of function settype  ----- */



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  TYPE
 *  Description:  
 * =====================================================================================
 */
	void
TYPE (int argc, char *argv[])
{
	struct types *p;

	if (argc > 2) {
		char *sep;

		printf("Usage: %s [", argv[0]);
		sep = " ";
		for (p = types; p->t_name; p++) {
			printf("%s%s", sep, p->t_name);
			sep = " | ";
		}
		printf(" ]\n");
		code = -1;
		return;
	}
	if (argc < 2) {
		printf("Using %s mode to transfer files.\n", typenm);
		code = 0;
		return;
	}
	settype(argv[1]);
}		/* -----  end of function TYPE  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ASCII
 *  Description:  
 * =====================================================================================
 */
	void
ASCII (void)
{
	settype("ascii");	
}		/* -----  end of function ASCII  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  BINARY
 *  Description:  
 * =====================================================================================
 */
	void
BINARY(void)
{
	settype("binary");	
}		/* -----  end of function BINAR----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  SYSTEM
 *  Description:  
 * =====================================================================================
 */
	void
SYSTEM (void)
{
	command("SYST");
}		/* -----  end of function SYSTEM  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  USER
 *  Description:  
 * =====================================================================================
 */
	void
USER (int argc, char *argv[])
{
	char *pass, *acct;
	char buf[MAXLOGIN];
	int n;

	if (argc == 1){
		printf("Name(%s): ", hostnm);
		if (fgets(buf, MAXLOGIN, stdin) == NULL){
			fprintf(stderr, "\nLogin failed.\n");
			return;
		}
		/* 将换行符变为结束符 */
		buf[strlen(buf) - 1] = '\0';
		if (*buf != '\0'){
			argc++;
			argv[1] = buf;
		}
	}	
	if (argc > 2) {
		printf("Usage: %s username\n", argv[0]);
		code = -1;
		return;
	}

	n = command("USER %s", argv[1]);
	if (n == CONTINUE) {
		pass = getpass("Password: ");
		n = command("PASS %s", pass);
	}
	if (n == CONTINUE) {
		acct = getpass("Account: ");
		n = command("ACCT %s", acct);
	}
	if (n != COMPLETE) {
		fprintf(stdout, "Login failed.\n");
		return;
	}	
}		/* -----  end of function USER  ----- */







