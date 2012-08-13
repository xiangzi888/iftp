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


#define MAXARGNUM 20 
#define MAXARGLEN 50


/*-----------------------------------------------------------------------------
 *  命令函数声明
 *-----------------------------------------------------------------------------*/
struct cmd *getcmd(char *name);
void quit(void);
void pwd(void);
void disconnect(void);
void ls(int argc, char *argv[]);


/*-----------------------------------------------------------------------------
 *  变量参数
 *-----------------------------------------------------------------------------*/
extern int margc;
extern char margv[MAXARGNUM][MAXARGLEN];

struct cmd {
	const char *c_name;	/* name of command */
	const char *c_help;	/* help string */
	char c_conn;		/* must be connected to use command */

    /* Exactly one of these should be non-NULL, function to call */
	void (*c_handler_1)(int, char **); 
	void (*c_handler_0)(void);
};


