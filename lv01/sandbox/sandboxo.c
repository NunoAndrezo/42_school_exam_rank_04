#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdint.h>
#include <string.h>

int	sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
	pid_t	pid;
	int		status;
	int		signal;
	int		exit_code;

	pid = fork();
	if (pid < 0)
		return -1;
	if (pid == 0)
	{
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
			return (0);
		}
		if (verbose) printf("Bad function: %s\n", strsignal(signal));
		return 0;
	}
	if (WIFEXITED(status))
	{
		exit_code = WEXITSTATUS(status);
		if (exit_code)
		{
			if (verbose) printf("Bad function: exited with code %d\n", exit_code);
			return 0;
		}
	}
	if (verbose) printf("Nice function!\n");
	return 1;
}




