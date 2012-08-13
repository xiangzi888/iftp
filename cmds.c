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

#include "ftp.h"
#include "cmds.h"


/*-----------------------------------------------------------------------------
 *  变量参数
 *-----------------------------------------------------------------------------*/
int margc;
char margv[MAXARGNUM][MAXARGLEN];

struct cmd cmdtab[] = {
	{ "quit",	"quitH",	0,	NULL,	quit},
	{ "ls",		"lsH",		0,	ls,		NULL},
	{ "pwd",	"pwdH",		1,	NULL,	pwd },
	{ "close",	"closeH",	1,	NULL,	disconnect},
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
 *         Name:  quit
 *  Description:  
 * =====================================================================================
 */
	void
quit (void)
{
	exit(0);
}		/* -----  end of function quit  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ls
 *  Description:  
 * =====================================================================================
 */
	void
ls (int argc, char *argv[])
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
}		/* -----  end of function ls  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  pwd
 *  Description:  
 * =====================================================================================
 */
	void
pwd (void)
{
	if (margc > 1)
		printf("Usage: %s\n", margv[0]);
	command("PWD");
}		/* -----  end of function pwd  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  disconnect
 *  Description:  
 * =====================================================================================
 */
	void
disconnect (void)
{	
	if (margc > 1)
		printf("Usage: %s\n", margv[0]);
	command("QUIT");
	if (cout) {
		(void) fclose(cout);
	}
	cout = NULL;
	connected = 0;
	data = -1;
}		/* -----  end of function disconnect  ----- */






