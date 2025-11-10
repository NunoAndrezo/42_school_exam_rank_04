#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int	ft_popen(const char *file, char *const argv[], char type)
{
	pid_t	pid;
	int		fd[2];

	if (!file || !argv || !type)
		return -1;
	if (type != 'w' && type != 'r')
		return -1;
	if (pipe(fd) == -1)
		return -1;
	pid = fork();
	if (pid < 0)
	{
		close(fd[0]);
		close(fd[1]);
		return -1;
	}
	if (!pid)
	{
		if (type == 'r')
		{
			close(fd[0]);
			if (dup2(fd[1], STDOUT_FILENO) == -1)
			{
				close(fd[1]);
				exit(1);
			}
			close(fd[1]);
		}
		else if (type == 'w')
		{
			close(fd[1]);
			if (dup2(fd[0], 0) == -1)
			{
				close(fd[0]);
				exit(1);
			}
			close(fd[0]);
		}
		execvp(file, argv);
		exit(1);
	}
	if (type == 'r')
	{
		close(fd[1]);
		return (fd[0]);
	}
	else if (type == 'w')
	{
		close(fd[0]);
		return (fd[1]);
	}
	return -1;
}