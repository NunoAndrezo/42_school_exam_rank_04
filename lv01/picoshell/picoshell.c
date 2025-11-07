#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

int picoshell(char **cmds[])
{
	int fd[2];
	int pid;
	int stat;
	int pfd = 0;

	for (size_t i = 0; cmds[i]; i++)
	{
		if (cmds[i + 1]) // not last command
		{
			if (pipe(fd) < 0)
				return 1;
		}
		else
		{
			fd[0] = -1;
			fd[1] = -1;
		}
		pid = fork();
		if (pid < 0) // err handle
		{
			for (size_t j = 0; j < 2; j++) // in case it's the last arg
				if (fd[j] != -1)
					close(fd[j]);
			return 1;
		}
		if (!pid) // child
		{
			if (pfd) // should ignore the first case
			{
				if (dup2(pfd, 0) < 0)
					exit(1);
				close(pfd);
			}
			if (fd[1] != -1)
			{
				if (dup2(fd[1], 1) < 0)
					exit(1);
				close(fd[1]);
				close(fd[0]); // close the fds either way
			}
			execvp(cmds[i][0], cmds[i]);
			exit(1); // exec fail
		}
		if (pfd)
			close(pfd);
		if (fd[1] != -1)
			close(fd[1]);
		pfd = fd[0];
	}
	while (wait(&stat) > 0) // wait for the program to finish
	{
		if (WIFEXITED(stat) && WEXITSTATUS(stat)) // if the exit sign is not 0 smth happened
			return 1;
		else if (!WIFEXITED(stat)) // the program did not terminated normally
			return 1;
	}
	return 0;
}