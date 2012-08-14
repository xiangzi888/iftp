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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/ftp.h>

#include "ftp.h"
#include "cmds.h"


/*-----------------------------------------------------------------------------
 *  变量参数
 *-----------------------------------------------------------------------------*/
int margc;
char *margv[MAXARGNUM];

struct cmd cmdtab[] = {
	{ "quit",	"HQUIT",	0,	NULL,	QUIT},
	{ "ls",		"HLS",		1,	LS,		NULL},
	{ "pwd",	"HPWD",		1,	NULL,	PWD},
	{ "close",	"HCLOSE",	1,	NULL,	CLOSE},
	{ "open",	"HOPEN",	0,	OPEN,	NULL},
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
	/*
	static char foo[2] = "-";
	const char *cmd;

	if (argc < 2) {
		argc++, argv[1] = NULL;
	}
	if (argc < 3) {
		argc++, argv[2] = foo;
	}
	if (argc > 3) {
		printf("Usage: %s [remote-dir] [local-file]\n", argv[0]);
		code = -1;
		return;
	}
	cmd = argv[0][0] == 'n' ? "NLST" : "LIST";
	if (strcmp(argv[2], "-") && (argv[2] = globulize(argv[2]))==NULL) {
		code = -1;
		return;
	}
	if (strcmp(argv[2], "-") && *argv[2] != '|')
		if ((argv[2] = globulize(argv[2]))==NULL || 
		    !confirm("output to local-file:", argv[2])) {
			code = -1;
			return;
	}
	recvrequest(cmd, argv[2], argv[1], "w", 0);*/
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
	char *host;

	if (argc != 2){
		printf("Usage: %s hostname\n", argv[0]);
		return;
	}
	if (connected) {
		printf("Already connected to %s, use close first.\n", hostname);
		return;
	}
	host = hookup(argv[1]);
	if (host)
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
			login(host, 1);
}		/* -----  end of function OPEN  ----- */






