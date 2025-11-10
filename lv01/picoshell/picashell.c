#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

int	picoshell(char **cmds[])
{
	pid_t	pid;
	int		status;
	int		fd[2];
	int		old_fd = 0;

	if (!cmds || !*cmds || !**cmds)
		return 1;
	for(int i = 0; cmds[i]; i++)
	{
		if (cmds[i + 1])
		{
			if (pipe(fd) == -1)
				return 1;
		}
		else
		{
			fd[0] = -1;
			fd[1] = -1;
		}
		pid = fork();
		if (pid < 0)
		{
			close(fd[0]);
			close(fd[1]);
			return 1;
		}
		if (!pid)
		{
			if (old_fd)
			{
				if (dup2(old_fd, STDIN_FILENO) == -1)
				{
					close(fd[0]);
					close(fd[1]);
					close(fd[old_fd]);
					exit(1);
				}
				close(old_fd);
			}
			if (fd[1] != -1)
			{
				close(fd[0]);
				if (dup2(fd[1], 1) == -1)
				{
					close(fd[0]);
					close(fd[1]);
					exit(1);
				}
				close(fd[1]);
			}
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		if (old_fd)
			close(old_fd);
		if (fd[1] != -1)
			close(fd[1]);
		old_fd = fd[0];
	}
	while (wait(&status) > 0)
	{
		if (WIFEXITED(status) && WEXITSTATUS(status))
			return 1;
	}
	return 0;
}