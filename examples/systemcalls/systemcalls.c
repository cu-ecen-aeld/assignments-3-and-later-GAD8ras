#include "systemcalls.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdarg.h>
#include <signal.h>
/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{
	if (cmd != NULL)
	{
		int ret = system(cmd);
		if(	(ret==-1)&&(ret==127) )
		{
			return false;
		}
		else
		{
			return true ;
		}
	}
	return false ;
}

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;

	int status;
	pid_t pid;
	pid = fork();
	
	if (pid == -1){
		va_end(args);
		return false;}
	
	else if (pid == 0) 
	{
		if (execv(command[0], command) == -1)
				exit(-1);
	}
	
	else
	{
		waitpid(pid, &status, 0);
		va_end(args);
		return WIFEXITED(status) && WEXITSTATUS(status) == 0;
	}

    return true;
}

bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
		printf("%s\n",command[i]);
    }
	//printf("%s\n",*command);

    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed

	//execlp( "/bin/sh", "/bin/sh", "-c", "%s > %s",command[0],outputfile, (char *)NULL );
	
	int status;
	pid_t pid;
	pid = fork();
	
	if (pid == -1){
		va_end(args);
		return false;}
	
	else if (pid == 0) 
	{
		int fd = open(outputfile, O_WRONLY | O_CREAT, 0644 );
		if (fd == -1){
			exit(-1);
		}

		if (dup2(fd,STDOUT_FILENO) == -1){
			exit(-1);
		}
		close(fd);

		if(execv(command[0], command) == -1)
			exit(-1);

	}
	else
		{
			waitpid(pid, &status, 0);
			va_end(args);
			return (WEXITSTATUS(status) && WIFEXITED(status) == 0);
		}

		return true;

}
