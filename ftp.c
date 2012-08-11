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

#include <stdlib.h>
#include <netdb.h>
#include <stdio.h>


/*-----------------------------------------------------------------------------
 *  变量定义
 *-----------------------------------------------------------------------------*/
 int ftpport;


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
