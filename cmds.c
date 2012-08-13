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
#include <stdlib.h>
#include <string.h>

#include "cmds.h"


/*-----------------------------------------------------------------------------
 *  变量参数
 *-----------------------------------------------------------------------------*/
int margc;
char margv[MAXARGNUM][MAXARGLEN];

struct cmd cmdtab[] = {
	{ "quit",		"quithelp",	0,	NULL,	quit},
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
