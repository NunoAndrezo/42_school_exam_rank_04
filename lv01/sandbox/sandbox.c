#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <signal.h>

int	sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
	pid_t	pid;
	int		status;
	int		exit_status;
	int		signal;

	pid = fork();
	if (pid < 0)
		return -1;
	if (pid == 0)
	{
		if (timeout)
			alarm(timeout);
		f();
		exit(0);
	}
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status))
	{
		signal = WTERMSIG(status);
		if (signal == SIGALRM)
		{
			if (verbose) printf("Bad function: timed out after %u seconds\n", timeout);
			return 0;
		}
		if (verbose) printf("Bad function: %s\n", strsignal(signal));
		return 0;
	if (WIFEXITED(status))
	{
		exit_status = WEXITSTATUS(status);
		if (exit_status == 0)
		{
			if (verbose) printf("Nice function!\n");
			return 1;
		}
		else
		{
			if (verbose) printf("Bad function: exited with code %d\n", exit_status);
			return 0;
		}
	}
	}
	return (1);
}