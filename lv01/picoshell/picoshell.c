#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int	picoshell(char **cmds[])
{
	int		status;
	int		pfd = 0;
	int		fd[2];
	pid_t	pid;

	if (!cmds || !*cmds || !**cmds)
		return (1);
	for(int i; cmds[i]; i++)
	{
		if (cmds[i + 1])
		{
			if (pipe(fd) < 0)
				return (1);
		}
		else
		{
			fd[0] = -1;
			fd[1] = -1;
		}
		pid = fork();
		if (pid < 0)
		{
			if (fd[1] != -1)
			{
				close(fd[0]);
				close(fd[1]);
				return (1);
			}
		}
		if (pid == 0)
		{
			if (pfd)
			{
				if (dup2(pfd, STDIN_FILENO) == -1)
					exit(1);
				close(pfd);
			}
			if (fd[1] != -1)
			{
				if (dup2(fd[1], STDOUT_FILENO) == -1)
					exit(1);
				close(fd[1]);
				close(fd[0]);
			}
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		if (pfd)
			close(pfd);
		if (fd[1] != -1)
			close(fd[1]);
		pfd = fd[0];
	}
	while (wait(&status) > 0)
	{
		if (WIFEXITED(status) && WEXITSTATUS(status))
			return (1);
	}
	return (0);
}