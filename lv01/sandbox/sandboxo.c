#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int	sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
	int		exit_status;
	int		status;
	int		signal_code;
	pid_t	pid;

	pid = fork();
	if (pid < 0)
		return -1;
	if (!pid)
	{
		alarm(timeout);
		f();
		exit(0);
	}
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status))
	{
		signal_code = WTERMSIG(status);
		if (signal_code == SIGALRM)
		{ 
			if (verbose)
				printf("Bad function: timed out after %u\n", timeout);
			return 0;
		}
		else if(signal_code)
		{
			if (verbose)
				printf("Bad function: %s\n", strsignal(signal_code));
			return 0;
		}
	}
	if (WIFEXITED(status))
	{
		exit_status = WEXITSTATUS(status);
		if (exit_status)
		{
			if (verbose)
				printf("Bad function: exited with code %d\n", exit_status);
			return 0;
		}
	}
	if (verbose)
		printf("Nice function!\n");
	return 1;
}