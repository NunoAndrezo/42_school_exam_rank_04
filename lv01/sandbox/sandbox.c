#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <signal.h>

int sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
	int pid;
	int ret;
	int stat;
	int es;
	int sig;

	pid = fork();
	if (pid < 0)
		return -1;
	if (!pid)
	{
		alarm(timeout);
		f();
		exit(0);
	}
	ret = waitpid(pid, &stat, 0);
	if (ret < 0)
		return -1;
	if (WIFSIGNALED(stat))
	{
		sig = WTERMSIG(stat);
		if (sig == SIGALRM)
		{
			if (verbose) printf("Bad function: timed out after %u seconds\n", timeout);
			return 0;
		}
		if (verbose) printf("Bad function: %s\n", strsignal(sig));
		return 0;
	}
	if (WIFEXITED(stat))
	{
		es = WEXITSTATUS(stat);
		if (es)
		{
			if (verbose) printf("Bad function: exited with code %u\n", es);
			return 0;
		}
		if (verbose) printf("Nice function!\n");
	}
	return 1;
}